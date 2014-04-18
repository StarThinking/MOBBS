// -*- mode:c++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
/* BE CAREFUL EDITING THIS FILE - it is a compilation of JNI
   machine-generated headers */

#include <jni.h>

#ifndef _Included_org_apache_hadoop_fs_ceph_CephTalker
#define _Included_org_apache_hadoop_fs_ceph_CephTalker
#ifdef __cplusplus
extern "C" {
#endif
  //these constants are machine-generated to match Java constants in the source
#undef org_apache_hadoop_fs_ceph_CephFileSystem_DEFAULT_BLOCK_SIZE
#define org_apache_hadoop_fs_ceph_CephFileSystem_DEFAULT_BLOCK_SIZE 8388608LL
#undef org_apache_hadoop_fs_ceph_CephInputStream_SKIP_BUFFER_SIZE
#define org_apache_hadoop_fs_ceph_CephInputStream_SKIP_BUFFER_SIZE 2048L
/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_initializeClient
 * Signature: (Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1initializeClient
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_getcwd
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1getcwd
  (JNIEnv *, jobject);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_setcwd
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1setcwd
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_rmdir
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1rmdir
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_unlink
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1unlink
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_rename
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1rename
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1exists
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_getblocksize
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1getblocksize
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_isdirectory
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1isdirectory
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_isfile
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1isfile
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_getdir
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1getdir
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_mkdirs
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1mkdirs
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_open_for_append
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1open_1for_1append
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_open_for_read
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1open_1for_1read
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_open_for_overwrite
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1open_1for_1overwrite
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_close
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1close
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_setPermission
 * Signature: (Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1setPermission
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_kill_client
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1kill_1client
  (JNIEnv *, jobject);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_stat
 * Signature: (Ljava/lang/String;Lorg/apache/hadoop/fs/ceph/CephFileSystem/Stat;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1stat
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_statfs
 * Signature: (Ljava/lang/String;Lorg/apache/hadoop/fs/ceph/CephFileSystem/CephStat;)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1statfs
(JNIEnv * env, jobject obj, jstring j_path, jobject j_cephstat);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_replication
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1replication
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_hosts
 * Signature: (IJ)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1hosts
  (JNIEnv *, jobject, jint, jlong);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_setTimes
 * Signature: (Ljava/lang/String;JJ)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1setTimes
  (JNIEnv *, jobject, jstring, jlong, jlong);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_read
 * Signature: (I[BII)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1read
  (JNIEnv *, jobject, jint, jbyteArray, jint, jint);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_seek_from_start
 * Signature: (IJ)J
 */
JNIEXPORT jlong JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1seek_1from_1start
  (JNIEnv *, jobject, jint, jlong);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_getpos
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1getpos
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_apache_hadoop_fs_ceph_CephTalker
 * Method:    ceph_write
 * Signature: (I[BII)I
 */
JNIEXPORT jint JNICALL Java_org_apache_hadoop_fs_ceph_CephTalker_ceph_1write
  (JNIEnv *, jobject, jint, jbyteArray, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
