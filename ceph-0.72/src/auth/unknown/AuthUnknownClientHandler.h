// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2009 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef CEPH_AUTHUNKNOWNCLIENTHANDLER_H
#define CEPH_AUTHUNKNOWNCLIENTHANDLER_H

#include "../AuthClientHandler.h"
#include "AuthUnknownProtocol.h"

class CephContext;

class AuthUnknownClientHandler : public AuthClientHandler {
public:
  AuthUnknownClientHandler(CephContext *cct_, RotatingKeyRing *rkeys) 
    : AuthClientHandler(cct_) {}

  void reset() { }

  int build_request(bufferlist& bl) { return 0; }
  int handle_response(int ret, bufferlist::iterator& iter) { return 0; }
  bool build_rotating_request(bufferlist& bl) { return false; }

  int get_protocol() { return CEPH_AUTH_UNKNOWN; }
  
  void tick() {}

  AuthAuthorizer *build_authorizer(uint32_t service_id) {
    AuthUnknownAuthorizer *auth = new AuthUnknownAuthorizer();
    if (auth) {
      auth->build_authorizer(cct->_conf->name, global_id);
    }
    return auth;
  }

  void validate_tickets() { }
  bool need_tickets() { return false; }

  void set_global_id(uint64_t id) { global_id = id; }
};

#endif
