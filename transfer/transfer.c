/*
 * Written by Sixiang Ma
 */

#include <rbd/librbd.h>
#include <stdio.h>
#include "./transfer.h"

#define SECTOR_SIZE 512
#define VBLOCK_SIZE 67108864
#define OBJECT_SIZE 4194304

int open(RBDState *s) {
    int r = 0;
    char *clientname = NULL;
    r = rados_create(&(s->cluster), clientname);
    if (r < 0) {
        printf("error initializing\n");
    }

    rados_conf_read_file(s->cluster, s->snap); 
    r = rados_connect(s->cluster);   
    if (r < 0) {
        printf("error connecting\n");
    }

    r = rados_ioctx_create(s->cluster, s->pool_name, &(s->io_ctx));
    if (r < 0) {
        printf("error opening pool %s\n", s->pool_name);
    }

    r = rbd_open(s->io_ctx, s->rbd_name, &(s->image), s->snap);
    if (r < 0) {
        printf("error reading header from %s\n", s->rbd_name);
    }
    
    rbd_image_info_t info;
    r = rbd_stat(s->image, &info, sizeof(info));
    if (r < 0) {
        printf("error rbd info\n");
    } else {
        printf("info.size = %ld\n", info.size);
    }
    return r;
}

void init(RBDState *hdd_rbd, RBDState *ssd_rbd) {
    int i;
    int obj_num = VBLOCK_SIZE/OBJECT_SIZE;
    int obj_sector_num = OBJECT_SIZE/SECTOR_SIZE;
    for(i=0; i<obj_num; i++) {  // write 64MB
       int sector_num = i * obj_sector_num;
       char num_char[10];
       sprintf(num_char, "%d", i);

       char *buf = (char *)malloc(OBJECT_SIZE * sizeof(char));
       strcpy(buf, "hello hdd-pool obj ");
       strcat(buf, num_char);
       rbd_write(hdd_rbd->image, sector_num, obj_sector_num, buf);
       free(buf);

       buf = (char *)malloc(OBJECT_SIZE * sizeof(char));
       strcpy(buf, "hello ssd-pool obj ");
       strcat(buf, num_char);
       rbd_write(ssd_rbd->image, sector_num, obj_sector_num, buf);
       free(buf);
    }
}

void transfer(RBDState *hdd_rbd, RBDState *ssd_rbd, int vblock_id) {
    int i;
    int obj_num = VBLOCK_SIZE/OBJECT_SIZE;
    int obj_sector_num = OBJECT_SIZE/SECTOR_SIZE;
    char *hdd_buf = (char *)malloc(VBLOCK_SIZE * sizeof(char));
    char *ssd_buf = (char *)malloc(VBLOCK_SIZE * sizeof(char));
    char *hdd_b = hdd_buf;
    char *ssd_b = ssd_buf;

    for(i=0; i<obj_num; i++) {  // read 64MB 
        int sector_num = i * obj_sector_num;
        rbd_read(hdd_rbd->image, sector_num, obj_sector_num, hdd_b);
        rbd_read(ssd_rbd->image, sector_num, obj_sector_num, ssd_b);
	//printf("buf = %s\n", b);
	hdd_b = hdd_b + OBJECT_SIZE;
	ssd_b = ssd_b + OBJECT_SIZE;
    }

    hdd_b = hdd_buf;
    ssd_b = ssd_buf;
    for(i=0; i<obj_num; i++) {  // write 64MB 
        int sector_num = i * obj_sector_num;
        rbd_write(ssd_rbd->image, sector_num, obj_sector_num, hdd_b);
        rbd_write(hdd_rbd->image, sector_num, obj_sector_num, ssd_b);
	//printf("buf = %s\n", b);
	hdd_b = hdd_b + OBJECT_SIZE;
	ssd_b = ssd_b + OBJECT_SIZE;
    }

    hdd_b = hdd_buf;
    ssd_b = ssd_buf;
    for(i=0; i<obj_num; i++) {  // read 64MB  to check exchange
        int sector_num = i * obj_sector_num;
        rbd_read(hdd_rbd->image, sector_num, obj_sector_num, hdd_b);
        rbd_read(ssd_rbd->image, sector_num, obj_sector_num, ssd_b);
	printf("hdd_buf = %s\n", hdd_b);
	printf("ssd_buf = %s\n", ssd_b);
	hdd_b = hdd_b + OBJECT_SIZE;
	ssd_b = ssd_b + OBJECT_SIZE;
    }
}

int main(int argc, const char **argv) {
    RBDState hdd_rbd, ssd_rbd;
    hdd_rbd.pool_name = "hdd-pool";
    hdd_rbd.rbd_name = "hdd-volume0";
    hdd_rbd.snap = NULL;
    ssd_rbd.pool_name = "ssd-pool";
    ssd_rbd.rbd_name = "ssd-volume0";
    ssd_rbd.snap = NULL;
    
    int r0 = open(&hdd_rbd);
    int r1 = open(&ssd_rbd);

    init(&hdd_rbd, &ssd_rbd);
    transfer(&hdd_rbd, &ssd_rbd, 0);

    return 0;
}


