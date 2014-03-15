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

#ifndef CEPH_MOSDPGTRIM_H
#define CEPH_MOSDPGTRIM_H

#include "msg/Message.h"

class MOSDPGTrim : public Message {
public:
  epoch_t epoch;
  pg_t pgid;
  eversion_t trim_to;

  epoch_t get_epoch() { return epoch; }

  MOSDPGTrim() : Message(MSG_OSD_PG_TRIM) {}
  MOSDPGTrim(version_t mv, pg_t p, eversion_t tt) :
    Message(MSG_OSD_PG_TRIM),
    epoch(mv), pgid(p), trim_to(tt) { }
private:
  ~MOSDPGTrim() {}

public:
  const char *get_type_name() const { return "pg_trim"; }
  void print(ostream& out) const {
    out << "pg_trim(" << pgid << " to " << trim_to << " e" << epoch << ")";
  }

  void encode_payload(uint64_t features) {
    ::encode(epoch, payload);
    ::encode(pgid, payload);
    ::encode(trim_to, payload);
  }
  void decode_payload() {
    bufferlist::iterator p = payload.begin();
    ::decode(epoch, p);
    ::decode(pgid, p);
    ::decode(trim_to, p);
  }
};

#endif
