// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef CEPH_MEXPORTDIRFINISH_H
#define CEPH_MEXPORTDIRFINISH_H

#include "msg/Message.h"

class MExportDirFinish : public Message {
  dirfrag_t dirfrag;

 public:
  dirfrag_t get_dirfrag() { return dirfrag; }
  
  MExportDirFinish() {}
  MExportDirFinish(dirfrag_t dirfrag) :
    Message(MSG_MDS_EXPORTDIRFINISH) {
    this->dirfrag = dirfrag;
  }
private:
  ~MExportDirFinish() {}

public:
  const char *get_type_name() const { return "ExFin"; }
  void print(ostream& o) const {
    o << "export_finish(" << dirfrag << ")";
  }
  
  void encode_payload(uint64_t features) {
    ::encode(dirfrag, payload);
  }
  void decode_payload() {
    bufferlist::iterator p = payload.begin();
    ::decode(dirfrag, p);
  }

};

#endif
