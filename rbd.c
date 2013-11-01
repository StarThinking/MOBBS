/*
 * QEMU Block driver for RADOS (Ceph)
 *
 * Copyright (C) 2010-2011 Christian Brunner <chb@muc.de>,
 *                         Josh Durgin <josh.durgin@dreamhost.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 * Contributions after 2012-01-13 are licensed under the terms of the
 * GNU GPL, version 2 or (at your option) any later version.
 */

#include <inttypes.h>

#include "qemu-common.h"
#include "qemu/error-report.h"
#include "block/block_int.h"

#include <rbd/librbd.h>

/*
 * When specifying the image filename use:
 *
 * rbd:poolname/devicename[@snapshotname][:option1=value1[:option2=value2...]]
 *
 * poolname must be the name of an existing rados pool.
 *
 * devicename is the name of the rbd image.
 *
 * Each option given is used to configure rados, and may be any valid
 * Ceph option, "id", or "conf".
 *
 * The "id" option indicates what user we should authenticate as to
 * the Ceph cluster.  If it is excluded we will use the Ceph default
 * (normally 'admin').
 *
 * The "conf" option specifies a Ceph configuration file to read.  If
 * it is not specified, we will read from the default Ceph locations
 * (e.g., /etc/ceph/ceph.conf).  To avoid reading _any_ configuration
 * file, specify conf=/dev/null.
 *
 * Configuration values containing :, @, or = can be escaped with a
 * leading "\".
 */

/* rbd_aio_discard added in 0.1.2 */
#if LIBRBD_VERSION_CODE >= LIBRBD_VERSION(0, 1, 2)
#define LIBRBD_SUPPORTS_DISCARD
#else
#undef LIBRBD_SUPPORTS_DISCARD
#endif

#define OBJ_MAX_SIZE (1UL << OBJ_DEFAULT_OBJ_ORDER)

#define RBD_MAX_CONF_NAME_SIZE 128
#define RBD_MAX_CONF_VAL_SIZE 512
#define RBD_MAX_CONF_SIZE 1024
#define RBD_MAX_POOL_NAME_SIZE 128
#define RBD_MAX_SNAP_NAME_SIZE 128
#define RBD_MAX_SNAPS 100

typedef enum {
    RBD_AIO_READ,
    RBD_AIO_WRITE,
    RBD_AIO_DISCARD,
    RBD_AIO_FLUSH
} RBDAIOCmd;

typedef struct RBDAIOCB {
    BlockDriverAIOCB common;
    QEMUBH *bh;
    int64_t ret;
    QEMUIOVector *qiov;
    char *bounce;
    RBDAIOCmd cmd;
    int64_t sector_num;
    int error;
    struct BDRVRBDState *s;
    int cancelled;
    int status;
} RBDAIOCB;

typedef struct RADOSCB {
    int rcbid;
    RBDAIOCB *acb;
    struct BDRVRBDState *s;
    int done;
    int64_t size;
    char *buf;
    int64_t ret;
} RADOSCB;

#define RBD_FD_READ 0
#define RBD_FD_WRITE 1

typedef struct BDRVRBDState {
    int fds[2];
    rados_t cluster;
    rados_ioctx_t io_ctx;
    rbd_image_t image;
    char name[RBD_MAX_IMAGE_NAME_SIZE];
    char *snap;
    int event_reader_pos;
    RADOSCB *event_rcb;
} BDRVRBDState;

#define HDD 0
#define SSD 1
#define MAX_BLOCK_NUM 1024
#define BLOCK_SIZE 67108864

typedef struct BLOCKITEM {
    int vblock_id;
    int block_type;
    int block_id;
    int read_num;
    int write_num;
} BLOCKITEM;

typedef struct VBDRVRBDState {
    struct BLOCKITEM table[MAX_BLOCK_NUM];        
    struct BDRVRBDState rbd0;
    struct BDRVRBDState rbd1;
} VBDRVRBDState;

static int64_t qemu_vrbd_getlength(BlockDriverState *bs);

static void init_blocktable(BlockDriverState *bs) {
    printf("init_blocktable\n");

    VBDRVRBDState *vs = bs->opaque;
    BLOCKITEM *table = vs->table;
    int64_t vblock_id;
    int64_t volume_byte_num = qemu_vrbd_getlength(bs);
    int64_t vblock_num = volume_byte_num / BLOCK_SIZE;
    printf("volume_byte_num = %ld, BLOCK_SIZE = 67108864, vblock_num = %ld\n", volume_byte_num, vblock_num);

    for( vblock_id = 0; vblock_id < vblock_num; vblock_id++ ) { 
        BLOCKITEM *item = &table[vblock_id];
	item->vblock_id = vblock_id;
        item->read_num = 0;
        item->write_num = 0;
        if( vblock_id < (vblock_num / 2) ) {
	    item->block_type = HDD;
	    item->block_id = vblock_id;
	 } else {
	    item->block_type = SSD;
	    item->block_id = vblock_id - (vblock_num / 2);
	 }
	 //printf("vblock_id = %d, block_id = %d, type = %d\n", item->vblock_id, item->block_id, item->block_type);
    }
}

static BLOCKITEM *get_blockitem( BlockDriverState *bs, int64_t vblock_sector_num ) {
    printf("getblockitem\n");
    VBDRVRBDState *vs = bs->opaque;
    BLOCKITEM *table = vs->table;
    int vblock_id = ( vblock_sector_num * 512 ) / BLOCK_SIZE;
    return &(table[vblock_id]);
}

static void rbd_aio_bh_cb(void *opaque);

static int qemu_rbd_next_tok(char *dst, int dst_len,
                             char *src, char delim,
                             const char *name,
                             char **p)
{
    int l;
    char *end;

    *p = NULL;

    if (delim != '\0') {
        for (end = src; *end; ++end) {
            if (*end == delim) {
                break;
            }
            if (*end == '\\' && end[1] != '\0') {
                end++;
            }
        }
        if (*end == delim) {
            *p = end + 1;
            *end = '\0';
        }
    }
    l = strlen(src);
    if (l >= dst_len) {
        error_report("%s too long", name);
        return -EINVAL;
    } else if (l == 0) {
        error_report("%s too short", name);
        return -EINVAL;
    }

    pstrcpy(dst, dst_len, src);

    return 0;
}

static void qemu_rbd_unescape(char *src)
{
    char *p;

    for (p = src; *src; ++src, ++p) {
        if (*src == '\\' && src[1] != '\0') {
            src++;
        }
        *p = *src;
    }
    *p = '\0';
}

static int qemu_rbd_parsename(const char *filename,
                              char *pool, int pool_len,
                              char *snap, int snap_len,
                              char *name, int name_len,
                              char *conf, int conf_len)
{
    const char *start;
    char *p, *buf;
    int ret;
    if (!strstart(filename, "rbd:", &start)) {
        return -EINVAL;
    }

    buf = g_strdup(start);
    p = buf;
    *snap = '\0';
    *conf = '\0';

    ret = qemu_rbd_next_tok(pool, pool_len, p, '/', "pool name", &p);
    if (ret < 0 || !p) {
        ret = -EINVAL;
        goto done;
    }
    qemu_rbd_unescape(pool);

    if (strchr(p, '@')) {
        ret = qemu_rbd_next_tok(name, name_len, p, '@', "object name", &p);
        if (ret < 0) {
            goto done;
        }
        ret = qemu_rbd_next_tok(snap, snap_len, p, ':', "snap name", &p);
        qemu_rbd_unescape(snap);
    } else {
        ret = qemu_rbd_next_tok(name, name_len, p, ':', "object name", &p);
    }
    qemu_rbd_unescape(name);
    if (ret < 0 || !p) {
        goto done;
    }

    ret = qemu_rbd_next_tok(conf, conf_len, p, '\0', "configuration", &p);

done:
    g_free(buf);
    return ret;
}

static char *qemu_rbd_parse_clientname(const char *conf, char *clientname)
{
    const char *p = conf;

    while (*p) {
        int len;
        const char *end = strchr(p, ':');

        if (end) {
            len = end - p;
        } else {
            len = strlen(p);
        }

        if (strncmp(p, "id=", 3) == 0) {
            len -= 3;
            strncpy(clientname, p + 3, len);
            clientname[len] = '\0';
            return clientname;
        }
        if (end == NULL) {
            break;
        }
        p = end + 1;
    }
    return NULL;
}

static int qemu_rbd_set_conf(rados_t cluster, const char *conf)
{
    char *p, *buf;
    char name[RBD_MAX_CONF_NAME_SIZE];
    char value[RBD_MAX_CONF_VAL_SIZE];
    int ret = 0;

    buf = g_strdup(conf);
    p = buf;

    while (p) {
        ret = qemu_rbd_next_tok(name, sizeof(name), p,
                                '=', "conf option name", &p);
        if (ret < 0) {
            break;
        }
        qemu_rbd_unescape(name);

        if (!p) {
            error_report("conf option %s has no value", name);
            ret = -EINVAL;
            break;
        }

        ret = qemu_rbd_next_tok(value, sizeof(value), p,
                                ':', "conf option value", &p);
        if (ret < 0) {
            break;
        }
        qemu_rbd_unescape(value);

        if (strcmp(name, "conf") == 0) {
            ret = rados_conf_read_file(cluster, value);
            if (ret < 0) {
                error_report("error reading conf file %s", value);
                break;
            }
        } else if (strcmp(name, "id") == 0) {
            /* ignore, this is parsed by qemu_rbd_parse_clientname() */
        } else {
            ret = rados_conf_set(cluster, name, value);
            if (ret < 0) {
                error_report("invalid conf option %s", name);
                ret = -EINVAL;
                break;
            }
        }
    }

    g_free(buf);
    return ret;
}

/*
 * This aio completion is being called from qemu_rbd_aio_event_reader()
 * and runs in qemu context. It schedules a bh, but just in case the aio
 * was not cancelled before.
 */
static void qemu_rbd_complete_aio(RADOSCB *rcb)
{
    RBDAIOCB *acb = rcb->acb;
    int64_t r;

    r = rcb->ret;

    if (acb->cmd != RBD_AIO_READ) {
        if (r < 0) {
            acb->ret = r;
            acb->error = 1;
        } else if (!acb->error) {
            acb->ret = rcb->size;
        }
    } else {
        if (r < 0) {
            memset(rcb->buf, 0, rcb->size);
            acb->ret = r;
            acb->error = 1;
        } else if (r < rcb->size) {
            memset(rcb->buf + r, 0, rcb->size - r);
            if (!acb->error) {
                acb->ret = rcb->size;
            }
        } else if (!acb->error) {
            acb->ret = r;
        }
    }
    /* Note that acb->bh can be NULL in case where the aio was cancelled */
    acb->bh = qemu_bh_new(rbd_aio_bh_cb, acb);
    qemu_bh_schedule(acb->bh);
    g_free(rcb);
}

/*
 * aio fd read handler. It runs in the qemu context and calls the
 * completion handling of completed rados aio operations.
 */
static void qemu_rbd_aio_event_reader(void *opaque)
{
    BDRVRBDState *s = opaque;

    ssize_t ret;

    do {
        char *p = (char *)&s->event_rcb;

        /* now read the rcb pointer that was sent from a non qemu thread */
        ret = read(s->fds[RBD_FD_READ], p + s->event_reader_pos,
                   sizeof(s->event_rcb) - s->event_reader_pos);
        if (ret > 0) {
            s->event_reader_pos += ret;
            if (s->event_reader_pos == sizeof(s->event_rcb)) {
                s->event_reader_pos = 0;
                qemu_rbd_complete_aio(s->event_rcb);
            }
        }
    } while (ret < 0 && errno == EINTR);
}

/* TODO Convert to fine grained options */
static QemuOptsList runtime_opts = {
    .name = "rbd",
    .head = QTAILQ_HEAD_INITIALIZER(runtime_opts.head),
    .desc = {
        {
            .name = "filename",
            .type = QEMU_OPT_STRING,
            .help = "Specification of the rbd image",
        },
        { /* end of list */ }
    },
};

static int qemu_rbd_open(BlockDriverState *bs, QDict *options, int flags,
                         Error **errp);

static int qemu_vrbd_open(BlockDriverState *bs, QDict *options, int flags,
                         Error **errp)
{	
    printf("qemu_vrbd_open\n");
    int ret0 = qemu_rbd_open(bs, options, flags, errp);
    //int ret1 = qemu_rbd_open(bs, options, flags, errp, 1);
    return ret0;
}

static int qemu_rbd_open(BlockDriverState *bs, QDict *options, int flags,
                         Error **errp)
{
    printf("qemu_rbd_open\n");
    VBDRVRBDState *vs = bs->opaque;
    BDRVRBDState *s0 = &(vs->rbd0);
    BDRVRBDState *s1 = &(vs->rbd1);

    char snap_buf0[RBD_MAX_SNAP_NAME_SIZE];
    char snap_buf1[RBD_MAX_SNAP_NAME_SIZE];
    char conf[RBD_MAX_CONF_SIZE];
    char clientname_buf[RBD_MAX_CONF_SIZE];
    char *clientname;
    char pool0[RBD_MAX_POOL_NAME_SIZE];
    char pool1[RBD_MAX_POOL_NAME_SIZE];
    QemuOpts *opts;
    Error *local_err = NULL;
    const char *filename;
    int r0, r1;

    opts = qemu_opts_create_nofail(&runtime_opts);
    qemu_opts_absorb_qdict(opts, options, &local_err);
    if (error_is_set(&local_err)) {
        qerror_report_err(local_err);
        error_free(local_err);
        qemu_opts_del(opts);
        return -EINVAL;
    }

    filename = qemu_opt_get(opts, "filename");
    if (qemu_rbd_parsename(filename, pool0, sizeof(pool0),
                           snap_buf0, sizeof(snap_buf0),
                           s0->name, sizeof(s0->name),
                           conf, sizeof(conf)) < 0) {
        r0 = -EINVAL;
        goto failed_opts;
    }

    clientname = qemu_rbd_parse_clientname(conf, clientname_buf);
    r0 = rados_create(&(s0->cluster), clientname);
    r1 = rados_create(&(s1->cluster), clientname);
    if (r0 < 0 || r1 < 0) {
        error_report("error initializing");
        goto failed_opts;
    }

    s0->snap = NULL;
    if (snap_buf0[0] != '\0') {
        s0->snap = g_strdup(snap_buf0);
    }

    s1->snap = NULL;
    if (snap_buf1[0] != '\0') {
        s1->snap = g_strdup(snap_buf1);
    }

    /*
     * Fallback to more conservative semantics if setting cache
     * options fails. Ignore errors from setting rbd_cache because the
     * only possible error is that the option does not exist, and
     * librbd defaults to no caching. If write through caching cannot
     * be set up, fall back to no caching.
     */
    if (flags & BDRV_O_NOCACHE) {
        rados_conf_set(s0->cluster, "rbd_cache", "false");
        rados_conf_set(s1->cluster, "rbd_cache", "false");
    } else {
        rados_conf_set(s0->cluster, "rbd_cache", "true");
        rados_conf_set(s1->cluster, "rbd_cache", "true");
    }

    if (strstr(conf, "conf=") == NULL) {
        /* try default location, but ignore failure */
        rados_conf_read_file(s0->cluster, NULL);
        rados_conf_read_file(s1->cluster, NULL);
    }
    if (conf[0] != '\0') {
        r0 = qemu_rbd_set_conf(s0->cluster, conf);
        r1 = qemu_rbd_set_conf(s1->cluster, conf);
        if (r0 < 0 || r1 < 0) {
            error_report("error setting config options");
            goto failed_shutdown;
        }
    }
    r0 = rados_connect(s0->cluster);
    r1 = rados_connect(s1->cluster);
    if (r0 < 0 || r1 < 0 ) {
        error_report("error connecting");
        goto failed_shutdown;
    }

    strcpy(pool0, "hdd-pool");
    strcpy(pool1, "ssd-pool");

    r0 = rados_ioctx_create(s0->cluster, pool0, &(s0->io_ctx));
    r1 = rados_ioctx_create(s1->cluster, pool1, &(s1->io_ctx));
    if (r0 < 0 || r1 < 0) {
        error_report("error opening pool0 %s, pool1 %s", pool0, pool1);
        goto failed_shutdown;
    }

    strcpy(s0->name, "hdd-volume0");
    strcpy(s1->name, "ssd-volume0");

    r0 = rbd_open(s0->io_ctx, s0->name, &(s0->image), s0->snap);
    r1 = rbd_open(s1->io_ctx, s1->name, &(s1->image), s1->snap);
    if (r0 < 0 || r1 < 0) {
        error_report("error reading header from %s, %s", s0->name, s1->name);
        goto failed_open;
    }

    bs->read_only = (s0->snap != NULL);

    s0->event_reader_pos = 0;
    s1->event_reader_pos = 0;
    r0 = qemu_pipe(s0->fds);
    r1 = qemu_pipe(s1->fds);
    if (r0 < 0 || r1 < 0) {
        error_report("error opening eventfd");
        goto failed;
    }
    fcntl(s0->fds[0], F_SETFL, O_NONBLOCK);
    fcntl(s0->fds[1], F_SETFL, O_NONBLOCK);
    fcntl(s1->fds[0], F_SETFL, O_NONBLOCK);
    fcntl(s1->fds[1], F_SETFL, O_NONBLOCK);
    qemu_aio_set_fd_handler(s0->fds[RBD_FD_READ], qemu_rbd_aio_event_reader,
                            NULL, s0);

    qemu_aio_set_fd_handler(s1->fds[RBD_FD_READ], qemu_rbd_aio_event_reader,
                            NULL, s1);
    qemu_opts_del(opts);

    printf("s0->name=%s, pool=%s\n", s0->name, pool0);
    printf("s1->name=%s, pool=%s\n", s1->name, pool1);

    init_blocktable(bs);

    return 0;

failed:
    rbd_close(s0->image);
    rbd_close(s1->image);
failed_open:
    rados_ioctx_destroy(s0->io_ctx);
    rados_ioctx_destroy(s1->io_ctx);
failed_shutdown:
    rados_shutdown(s0->cluster);
    rados_shutdown(s1->cluster);
    g_free(s0->snap);
    g_free(s1->snap);
failed_opts:
    qemu_opts_del(opts);
    return (r0 && r1);
}

static void qemu_rbd_close(BlockDriverState *bs, int rbdnum);

static void qemu_vrbd_close(BlockDriverState *bs)
{
   printf("qemu_vrbd_close\n");
   qemu_rbd_close(bs, 0);
   qemu_rbd_close(bs, 1);
}

static void qemu_rbd_close(BlockDriverState *bs, int rbdnum)
{
    VBDRVRBDState *vs = bs->opaque;
    BDRVRBDState *s;

    if(rbdnum == 0)
        s = &(vs->rbd0);
    else
        s = &(vs->rbd1);

    close(s->fds[0]);
    close(s->fds[1]);
    qemu_aio_set_fd_handler(s->fds[RBD_FD_READ], NULL, NULL, NULL);

    rbd_close(s->image);
    rados_ioctx_destroy(s->io_ctx);
    g_free(s->snap);
    rados_shutdown(s->cluster);
}

/*
 * Cancel aio. Since we don't reference acb in a non qemu threads,
 * it is safe to access it here.
 */
static void qemu_rbd_aio_cancel(BlockDriverAIOCB *blockacb)
{
    RBDAIOCB *acb = (RBDAIOCB *) blockacb;
    acb->cancelled = 1;

    while (acb->status == -EINPROGRESS) {
        qemu_aio_wait();
    }

    qemu_aio_release(acb);
}

static const AIOCBInfo rbd_aiocb_info = {
    .aiocb_size = sizeof(RBDAIOCB),
    .cancel = qemu_rbd_aio_cancel,
};

static int qemu_rbd_send_pipe(BDRVRBDState *s, RADOSCB *rcb)
{
    int ret = 0;
    while (1) {
        fd_set wfd;
        int fd = s->fds[RBD_FD_WRITE];

        /* send the op pointer to the qemu thread that is responsible
           for the aio/op completion. Must do it in a qemu thread context */
        ret = write(fd, (void *)&rcb, sizeof(rcb));
        if (ret >= 0) {
            break;
        }
        if (errno == EINTR) {
            continue;
        }
        if (errno != EAGAIN) {
            break;
        }

        FD_ZERO(&wfd);
        FD_SET(fd, &wfd);
        do {
            ret = select(fd + 1, NULL, &wfd, NULL, NULL);
        } while (ret < 0 && errno == EINTR);
    }

    return ret;
}

/*
 * This is the callback function for rbd_aio_read and _write
 *
 * Note: this function is being called from a non qemu thread so
 * we need to be careful about what we do here. Generally we only
 * write to the block notification pipe, and do the rest of the
 * io completion handling from qemu_rbd_aio_event_reader() which
 * runs in a qemu context.
 */
static void rbd_finish_aiocb(rbd_completion_t c, RADOSCB *rcb)
{
    int ret;
    rcb->ret = rbd_aio_get_return_value(c);
    rbd_aio_release(c);
    ret = qemu_rbd_send_pipe(rcb->s, rcb);
    if (ret < 0) {
        error_report("failed writing to acb->s->fds");
        g_free(rcb);
    }
}

/* Callback when all queued rbd_aio requests are complete */

static void rbd_aio_bh_cb(void *opaque)
{
    RBDAIOCB *acb = opaque;

    if (acb->cmd == RBD_AIO_READ) {
        qemu_iovec_from_buf(acb->qiov, 0, acb->bounce, acb->qiov->size);
    }
    qemu_vfree(acb->bounce);
    acb->common.cb(acb->common.opaque, (acb->ret > 0 ? 0 : acb->ret));
    qemu_bh_delete(acb->bh);
    acb->bh = NULL;
    acb->status = 0;

    if (!acb->cancelled) {
        qemu_aio_release(acb);
    }
}

static int rbd_aio_discard_wrapper(rbd_image_t image,
                                   uint64_t off,
                                   uint64_t len,
                                   rbd_completion_t comp)
{
#ifdef LIBRBD_SUPPORTS_DISCARD
    return rbd_aio_discard(image, off, len, comp);
#else
    return -ENOTSUP;
#endif
}

static int rbd_aio_flush_wrapper(rbd_image_t image,
                                 rbd_completion_t comp)
{
#ifdef LIBRBD_SUPPORTS_AIO_FLUSH
    return rbd_aio_flush(image, comp);
#else
    return -ENOTSUP;
#endif
}

static BlockDriverAIOCB *rbd_start_aio(BlockDriverState *bs,
                                       int64_t sector_num,
                                       QEMUIOVector *qiov,
                                       int nb_sectors,
                                       BlockDriverCompletionFunc *cb,
                                       void *opaque,
                                       RBDAIOCmd cmd,
				       int rbdnum)
{
    RBDAIOCB *acb;
    RADOSCB *rcb;
    rbd_completion_t c;
    int64_t off, size;
    char *buf;
    int r;

    VBDRVRBDState *vs = bs->opaque;
    BDRVRBDState *s;

    if(rbdnum == 0)
        s = &(vs->rbd0);
    else
        s = &(vs->rbd1);

    acb = qemu_aio_get(&rbd_aiocb_info, bs, cb, opaque);
    acb->cmd = cmd;
    acb->qiov = qiov;
    if (cmd == RBD_AIO_DISCARD || cmd == RBD_AIO_FLUSH) {
        acb->bounce = NULL;
    } else {
        acb->bounce = qemu_blockalign(bs, qiov->size);
    }
    acb->ret = 0;
    acb->error = 0;
    acb->s = s;
    acb->cancelled = 0;
    acb->bh = NULL;
    acb->status = -EINPROGRESS;

    if (cmd == RBD_AIO_WRITE) {
        qemu_iovec_to_buf(acb->qiov, 0, acb->bounce, qiov->size);
    }

    buf = acb->bounce;

    off = sector_num * BDRV_SECTOR_SIZE;
    size = nb_sectors * BDRV_SECTOR_SIZE;

    rcb = g_malloc(sizeof(RADOSCB));
    rcb->done = 0;
    rcb->acb = acb;
    rcb->buf = buf;
    rcb->s = acb->s;
    rcb->size = size;
    r = rbd_aio_create_completion(rcb, (rbd_callback_t) rbd_finish_aiocb, &c);
    if (r < 0) {
        goto failed;
    }

    switch (cmd) {
    case RBD_AIO_WRITE:
        r = rbd_aio_write(s->image, off, size, buf, c);
        break;
    case RBD_AIO_READ:
        r = rbd_aio_read(s->image, off, size, buf, c);
        break;
    case RBD_AIO_DISCARD:
        r = rbd_aio_discard_wrapper(s->image, off, size, c);
        break;
    case RBD_AIO_FLUSH:
        r = rbd_aio_flush_wrapper(s->image, c);
        break;
    default:
        r = -EINVAL;
    }

    if (r < 0) {
        goto failed;
    }

    return &acb->common;

failed:
    g_free(rcb);
    qemu_aio_release(acb);
    return NULL;
}

static BlockDriverAIOCB *qemu_vrbd_aio_readv(BlockDriverState *bs,
                                            int64_t vblock_sector_num,
                                            QEMUIOVector *qiov,
                                            int nb_sectors,
                                            BlockDriverCompletionFunc *cb,
                                            void *opaque)
{
    BLOCKITEM *item = get_blockitem( bs, vblock_sector_num );
    int block_type = item -> block_type;
    int64_t block_id = item -> block_id;
    int64_t sector_num = ((block_id * BLOCK_SIZE) + ((vblock_sector_num * 512) % BLOCK_SIZE)) / 512;
    printf("qemu_vrbd_aio_readv: v_sector_num = %ld, sector_num = %ld, block_id = %d, block_type = %d\n", vblock_sector_num, sector_num,  block_id, block_type);
    return rbd_start_aio(bs, sector_num, qiov, nb_sectors, cb, opaque,
                         RBD_AIO_READ, block_type);
}

static BlockDriverAIOCB *qemu_vrbd_aio_writev(BlockDriverState *bs,
                                             int64_t vblock_sector_num,
                                             QEMUIOVector *qiov,
                                             int nb_sectors,
                                             BlockDriverCompletionFunc *cb,
                                             void *opaque)
{
    
    BLOCKITEM *item = get_blockitem( bs, vblock_sector_num );
    int block_type = item -> block_type;
    int64_t block_id = item -> block_id;
    int64_t sector_num = ((block_id * BLOCK_SIZE) + ((vblock_sector_num * 512) % BLOCK_SIZE)) / 512;
    printf("qemu_vrbd_aio_writev: v_sector_num = %ld, sector_num = %ld, block_id = %d, block_type = %d\n", vblock_sector_num, sector_num,  block_id, block_type);
    return rbd_start_aio(bs, sector_num, qiov, nb_sectors, cb, opaque,
                         RBD_AIO_WRITE, block_type);
}

static BlockDriverAIOCB *qemu_vrbd_aio_flush(BlockDriverState *bs,
                                            BlockDriverCompletionFunc *cb,
                                            void *opaque)
{
    printf("qemu_vrbd_aio_flush\n");
    return (rbd_start_aio(bs, 0, NULL, 0, cb, opaque, RBD_AIO_FLUSH, 1) && rbd_start_aio(bs, 0, NULL, 0, cb, opaque, RBD_AIO_FLUSH, 0));
}

static int qemu_vrbd_getinfo(BlockDriverState *bs, BlockDriverInfo *bdi)
{
    printf("qemu_vrbd_getinfo\n");
    VBDRVRBDState *vs = bs->opaque;
    BDRVRBDState *s0 = &(vs->rbd0);
    BDRVRBDState *s1 = &(vs->rbd1);

    rbd_image_info_t info0, info1;
    int r0, r1;

    r0 = rbd_stat(s0->image, &info0, sizeof(info0));
    r1 = rbd_stat(s1->image, &info1, sizeof(info1));
    if (r0 < 0) {
        return r0;
    } else if (r1 < 0) {
        return r1;
    }

    bdi->cluster_size = (info0.obj_size + info1.obj_size);
    return 0;
}

static int64_t qemu_vrbd_getlength(BlockDriverState *bs)
{
    printf("qemu_vrbd_getlength\n");
    VBDRVRBDState *vs = bs->opaque;
    BDRVRBDState *s0 = &(vs->rbd0);
    BDRVRBDState *s1 = &(vs->rbd1);

    rbd_image_info_t info0, info1;
    int r0, r1;
    r0 = rbd_stat(s0->image, &info0, sizeof(info0));
    r1 = rbd_stat(s1->image, &info1, sizeof(info1));
    if (r0 < 0) {
        return r0;
    } else if (r1 < 0) {
        return r1;
    }

    return (info0.size + info1.size);
}

/*
static int qemu_rbd_truncate(BlockDriverState *bs, int64_t offset)
{
    VBDRVRBDState *vs = bs->opaque;
    BDRVRBDState *s = &(vs->rbd0);
    int r;

    r = rbd_resize(s->image, offset);
    if (r < 0) {
        return r;
    }

    return 0;
}

#ifdef LIBRBD_SUPPORTS_DISCARD
static BlockDriverAIOCB* qemu_rbd_aio_discard(BlockDriverState *bs,
                                              int64_t sector_num,
                                              int nb_sectors,
                                              BlockDriverCompletionFunc *cb,
                                              void *opaque)
{
    return rbd_start_aio(bs, sector_num, NULL, nb_sectors, cb, opaque,
                         RBD_AIO_DISCARD);
}
#endif*/

static QEMUOptionParameter qemu_vrbd_create_options[] = {
    {
     .name = BLOCK_OPT_SIZE,
     .type = OPT_SIZE,
     .help = "Virtual disk size"
    },
    {
     .name = BLOCK_OPT_CLUSTER_SIZE,
     .type = OPT_SIZE,
     .help = "RBD object size"
    },
    {NULL}
};

static BlockDriver bdrv_vrbd = {
    .format_name        = "rbd",
    .instance_size      = sizeof(VBDRVRBDState),
    .bdrv_needs_filename = true,
    .bdrv_file_open     = qemu_vrbd_open,
    .bdrv_close         = qemu_vrbd_close,
    .bdrv_has_zero_init = bdrv_has_zero_init_1,
    .bdrv_get_info      = qemu_vrbd_getinfo,
    .create_options     = qemu_vrbd_create_options,
    .bdrv_getlength     = qemu_vrbd_getlength,
   // .bdrv_truncate      = qemu_vrbd_truncate,
    .protocol_name      = "rbd",

    .bdrv_aio_readv         = qemu_vrbd_aio_readv,
    .bdrv_aio_writev        = qemu_vrbd_aio_writev,

    .bdrv_aio_flush         = qemu_vrbd_aio_flush,

#ifdef LIBRBD_SUPPORTS_DISCARD
   // .bdrv_aio_discard       = qemu_vrbd_aio_discard,
#endif

};

static void bdrv_rbd_init(void)
{
    bdrv_register(&bdrv_vrbd);
}

block_init(bdrv_rbd_init);
