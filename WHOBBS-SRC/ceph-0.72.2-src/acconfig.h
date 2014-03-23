/* src/acconfig.h.  Generated from acconfig.h.in by configure.  */
/* src/acconfig.h.in.  Generated from configure.ac by autoheader.  */

/* fallocate(2) is supported */
#define CEPH_HAVE_FALLOCATE /**/

/* Define if darwin/osx */
/* #undef DARWIN */

/* Define if you want C_Gather debugging */
#define DEBUG_GATHER 1

/* Define if enabling coverage. */
/* #undef ENABLE_COVERAGE */

/* FastCGI headers are in /usr/include/fastcgi */
/* #undef FASTCGI_INCLUDE_DIR */

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* have boost::random::discrete_distribution */
/* #undef HAVE_BOOST_RANDOM_DISCRETE_DISTRIBUTION */

/* Define if have curl_multi_wait() */
/* #undef HAVE_CURL_MULTI_WAIT */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* linux/fiemap.h was found, fiemap ioctl will be used */
#define HAVE_FIEMAP_H /**/

/* Define to 1 if you have the `fuse_getgroups' function. */
/* #undef HAVE_FUSE_GETGROUPS */

/* we have a recent yasm and are x86_64 */
/* #undef HAVE_GOOD_YASM_ELF64 */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Defined if LevelDB supports bloom filters */
/* #undef HAVE_LEVELDB_FILTER_POLICY */

/* Defined if you don't have atomic_ops */
#define HAVE_LIBAIO 1

/* Define to 1 if you have the `boost_system' library (-lboost_system). */
/* #undef HAVE_LIBBOOST_SYSTEM */

/* Define to 1 if you have the `boost_system-mt' library (-lboost_system-mt).
   */
/* #undef HAVE_LIBBOOST_SYSTEM_MT */

/* Define to 1 if you have the `boost_thread' library (-lboost_thread). */
/* #undef HAVE_LIBBOOST_THREAD */

/* Define to 1 if you have the `boost_thread-mt' library (-lboost_thread-mt).
   */
#define HAVE_LIBBOOST_THREAD_MT 1

/* Define if you have fuse */
/* #undef HAVE_LIBFUSE */

/* Define to 1 if you have the `leveldb' library (-lleveldb). */
#define HAVE_LIBLEVELDB 1

/* Define to 1 if you have the `profiler' library (-lprofiler). */
/* #undef HAVE_LIBPROFILER */

/* Define to 1 if you have the `snappy' library (-lsnappy). */
#define HAVE_LIBSNAPPY 1

/* Define if you have tcmalloc */
#define HAVE_LIBTCMALLOC 1

/* Defined if you have libzfs enabled */
/* #undef HAVE_LIBZFS */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the `prctl' function. */
#define HAVE_PRCTL 1

/* Define if you have perftools profiler enabled */
/* #undef HAVE_PROFILER */

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `syncfs' function. */
#define HAVE_SYNCFS 1

/* sync_file_range(2) is supported */
#define HAVE_SYNC_FILE_RANGE /**/

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/prctl.h> header file. */
#define HAVE_SYS_PRCTL_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* we have syncfs */
#define HAVE_SYS_SYNCFS 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/vfs.h> header file. */
#define HAVE_SYS_VFS_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <sys/xattr.h> header file. */
#define HAVE_SYS_XATTR_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Defined if you do not have atomic_ops */
/* #undef NO_ATOMIC_OPS */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "ceph"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "ceph-devel@vger.kernel.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "ceph"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ceph 0.72.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ceph"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.72.2"

/* Defined if you want pg ref debugging */
/* #undef PG_DEBUG_REFS */

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if using CryptoPP. */
#define USE_CRYPTOPP 1

/* Define if using NSS. */
/* #undef USE_NSS */

/* Version number of package */
#define VERSION "0.72.2"

/* define if radosgw enabled */
#define WITH_RADOSGW 1
