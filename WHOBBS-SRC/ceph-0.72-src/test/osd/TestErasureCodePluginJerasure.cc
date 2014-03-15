// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2013 Cloudwatt <libre.licensing@cloudwatt.com>
 *
 * Author: Loic Dachary <loic@dachary.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 * 
 */

#include <errno.h>
#include "global/global_init.h"
#include "osd/ErasureCodePlugin.h"
#include "common/ceph_argparse.h"
#include "global/global_context.h"
#include "gtest/gtest.h"

TEST(ErasureCodePlugin, factory)
{
  ErasureCodePluginRegistry &instance = ErasureCodePluginRegistry::instance();
  map<std::string,std::string> parameters;
  parameters["erasure-code-directory"] = ".libs";
  {
    ErasureCodeInterfaceRef erasure_code;
    EXPECT_FALSE(erasure_code);
    EXPECT_EQ(-ENOENT, instance.factory("jerasure", parameters, &erasure_code));
    EXPECT_FALSE(erasure_code);
  }
  const char *techniques[] = {
    "reed_sol_van",
    "reed_sol_r6_op",
    "cauchy_orig",
    "cauchy_good",
    "liberation",
    "blaum_roth",
    "liber8tion",
    0
  };
  for(const char **technique = techniques; *technique; technique++) {
    ErasureCodeInterfaceRef erasure_code;
    parameters["erasure-code-technique"] = *technique;
    EXPECT_FALSE(erasure_code);
    EXPECT_EQ(0, instance.factory("jerasure", parameters, &erasure_code));
    EXPECT_TRUE(erasure_code);
  }
}

int main(int argc, char **argv)
{
  vector<const char*> args;
  argv_to_vec(argc, (const char **)argv, args);

  global_init(NULL, args, CEPH_ENTITY_TYPE_CLIENT, CODE_ENVIRONMENT_UTILITY, 0);
  common_init_finish(g_ceph_context);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

/* 
 * Local Variables:
 * compile-command: "cd ../.. ; make -j4 && 
 *   make unittest_erasure_code_plugin_jerasure && 
 *   valgrind --tool=memcheck ./unittest_erasure_code_plugin_jerasure \
 *      --gtest_filter=*.* --log-to-stderr=true --debug-osd=20"
 * End:
 */

