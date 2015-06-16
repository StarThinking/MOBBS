// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
#include "include/rados/librados.h"
#include "include/rados/librados.hpp"
#include "test/librados/test.h"

#include "gtest/gtest.h"
#include <errno.h>
#include <string>

using namespace librados;

TEST(LibRadosList, ListObjects) {
  char buf[128];
  rados_t cluster;
  rados_ioctx_t ioctx;
  std::string pool_name = get_temp_pool_name();
  ASSERT_EQ("", create_one_pool(pool_name, &cluster));
  rados_ioctx_create(cluster, pool_name.c_str(), &ioctx);
  memset(buf, 0xcc, sizeof(buf));
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo", buf, sizeof(buf), 0));
  rados_list_ctx_t ctx;
  ASSERT_EQ(0, rados_objects_list_open(ioctx, &ctx));
  const char *entry;
  bool foundit = false;
  while (rados_objects_list_next(ctx, &entry, NULL) != -ENOENT) {
    foundit = true;
    ASSERT_EQ(std::string(entry), "foo");
  }
  ASSERT_TRUE(foundit);
  rados_objects_list_close(ctx);
  rados_ioctx_destroy(ioctx);
  ASSERT_EQ(0, destroy_one_pool(pool_name, &cluster));
}

TEST(LibRadosList, ListObjectsPP) {
  std::string pool_name = get_temp_pool_name();
  Rados cluster;
  ASSERT_EQ("", create_one_pool_pp(pool_name, cluster));
  IoCtx ioctx;
  cluster.ioctx_create(pool_name.c_str(), ioctx);
  char buf[128];
  memset(buf, 0xcc, sizeof(buf));
  bufferlist bl1;
  bl1.append(buf, sizeof(buf));
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo", bl1, sizeof(buf), 0));
  ObjectIterator iter(ioctx.objects_begin());
  bool foundit = false;
  while (iter != ioctx.objects_end()) {
    foundit = true;
    ASSERT_EQ((*iter).first, "foo");
    ++iter;
  }
  ASSERT_TRUE(foundit);
  ioctx.close();
  ASSERT_EQ(0, destroy_one_pool_pp(pool_name, cluster));
}

static void check_list(std::set<std::string>& myset, rados_list_ctx_t& ctx)
{
  const char *entry;
  std::set<std::string> orig_set(myset);
  /**
   * During splitting, we might see duplicate items.
   * We assert that every object returned is in myset and that
   * we don't hit ENOENT until we have hit every item in myset
   * at least once.
   */
  while (rados_objects_list_next(ctx, &entry, NULL) != -ENOENT) {
    ASSERT_TRUE(orig_set.end() != orig_set.find(std::string(entry)));
    myset.erase(std::string(entry));
  }
  ASSERT_TRUE(myset.empty());
}

TEST(LibRadosList, ListObjectsNS) {
  char buf[128];
  rados_t cluster;
  rados_ioctx_t ioctx;
  std::string pool_name = get_temp_pool_name();
  ASSERT_EQ("", create_one_pool(pool_name, &cluster));
  rados_ioctx_create(cluster, pool_name.c_str(), &ioctx);
  memset(buf, 0xcc, sizeof(buf));
  // Create :foo1, :foo2, :foo3, n1:foo1, ns1:foo4, ns1:foo5, ns2:foo6, n2:foo7
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo1", buf, sizeof(buf), 0));
  rados_ioctx_set_namespace(ioctx, "ns1");
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo1", buf, sizeof(buf), 0));
  rados_ioctx_set_namespace(ioctx, "");
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo2", buf, sizeof(buf), 0));
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo3", buf, sizeof(buf), 0));
  rados_ioctx_set_namespace(ioctx, "ns1");
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo4", buf, sizeof(buf), 0));
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo5", buf, sizeof(buf), 0));
  rados_ioctx_set_namespace(ioctx, "ns2");
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo6", buf, sizeof(buf), 0));
  ASSERT_EQ((int)sizeof(buf), rados_write(ioctx, "foo7", buf, sizeof(buf), 0));

  std::set<std::string> def, ns1, ns2;
  def.insert(std::string("foo1"));
  def.insert(std::string("foo2"));
  def.insert(std::string("foo3"));
  ns1.insert(std::string("foo1"));
  ns1.insert(std::string("foo4"));
  ns1.insert(std::string("foo5"));
  ns2.insert(std::string("foo6"));
  ns2.insert(std::string("foo7"));

  rados_list_ctx_t ctx;
  // Check default namespace ""
  rados_ioctx_set_namespace(ioctx, "");
  ASSERT_EQ(0, rados_objects_list_open(ioctx, &ctx));
  check_list(def, ctx);
  rados_objects_list_close(ctx);

  // Check default namespace "ns1"
  rados_ioctx_set_namespace(ioctx, "ns1");
  ASSERT_EQ(0, rados_objects_list_open(ioctx, &ctx));
  check_list(ns1, ctx);
  rados_objects_list_close(ctx);

  // Check default namespace "ns2"
  rados_ioctx_set_namespace(ioctx, "ns2");
  ASSERT_EQ(0, rados_objects_list_open(ioctx, &ctx));
  check_list(ns2, ctx);
  rados_objects_list_close(ctx);

  rados_ioctx_destroy(ioctx);
  ASSERT_EQ(0, destroy_one_pool(pool_name, &cluster));
}

static void check_listpp(std::set<std::string>& myset, IoCtx& ioctx)
{
  ObjectIterator iter(ioctx.objects_begin());
  std::set<std::string> orig_set(myset);
  /**
   * During splitting, we might see duplicate items.
   * We assert that every object returned is in myset and that
   * we don't hit ENOENT until we have hit every item in myset
   * at least once.
   */
  while (iter != ioctx.objects_end()) {
    ASSERT_TRUE(orig_set.end() != orig_set.find(std::string((*iter).first)));
    myset.erase(std::string((*iter).first));
    ++iter;
  }
  ASSERT_TRUE(myset.empty());
}

TEST(LibRadosList, ListObjectsPPNS) {
  std::string pool_name = get_temp_pool_name();
  Rados cluster;
  ASSERT_EQ("", create_one_pool_pp(pool_name, cluster));
  IoCtx ioctx;
  cluster.ioctx_create(pool_name.c_str(), ioctx);
  char buf[128];
  memset(buf, 0xcc, sizeof(buf));
  bufferlist bl1;
  bl1.append(buf, sizeof(buf));
  // Create :foo1, :foo2, :foo3, n1:foo1, ns1:foo4, ns1:foo5, ns2:foo6, n2:foo7
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo1", bl1, sizeof(buf), 0));
  ioctx.set_namespace("ns1");
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo1", bl1, sizeof(buf), 0));
  ioctx.set_namespace("");
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo2", bl1, sizeof(buf), 0));
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo3", bl1, sizeof(buf), 0));
  ioctx.set_namespace("ns1");
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo4", bl1, sizeof(buf), 0));
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo5", bl1, sizeof(buf), 0));
  ioctx.set_namespace("ns2");
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo6", bl1, sizeof(buf), 0));
  ASSERT_EQ((int)sizeof(buf), ioctx.write("foo7", bl1, sizeof(buf), 0));

  std::set<std::string> def, ns1, ns2;
  def.insert(std::string("foo1"));
  def.insert(std::string("foo2"));
  def.insert(std::string("foo3"));
  ns1.insert(std::string("foo1"));
  ns1.insert(std::string("foo4"));
  ns1.insert(std::string("foo5"));
  ns2.insert(std::string("foo6"));
  ns2.insert(std::string("foo7"));

  ioctx.set_namespace("");
  check_listpp(def, ioctx);

  ioctx.set_namespace("ns1");
  check_listpp(ns1, ioctx);

  ioctx.set_namespace("ns2");
  check_listpp(ns2, ioctx);

  ioctx.close();
  ASSERT_EQ(0, destroy_one_pool_pp(pool_name, cluster));
}
