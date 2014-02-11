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


#ifndef CEPH_MOSDPGLOG_H
#define CEPH_MOSDPGLOG_H

#include "msg/Message.h"

class MOSDPGLog : public Message {

  static const int HEAD_VERSION = 3;
  static const int COMPAT_VERSION = 2;

  epoch_t epoch;
  /// query_epoch is the epoch of the query being responded to, or
  /// the current epoch if this is not being sent in response to a
  /// query. This allows the recipient to disregard responses to old
  /// queries.
  epoch_t query_epoch;

public:
  pg_info_t info;
  pg_log_t log;
  pg_missing_t missing;
  pg_interval_map_t past_intervals;

  epoch_t get_epoch() { return epoch; }
  pg_t get_pgid() { return info.pgid; }
  epoch_t get_query_epoch() { return query_epoch; }

  MOSDPGLog() : Message(MSG_OSD_PG_LOG, HEAD_VERSION, COMPAT_VERSION) { }
  MOSDPGLog(version_t mv, pg_info_t& i)
    : Message(MSG_OSD_PG_LOG, HEAD_VERSION, COMPAT_VERSION),
      epoch(mv), query_epoch(mv), info(i)  { }
  MOSDPGLog(version_t mv, pg_info_t& i, epoch_t query_epoch)
    : Message(MSG_OSD_PG_LOG, HEAD_VERSION, COMPAT_VERSION),
      epoch(mv), query_epoch(query_epoch), info(i)  { }

private:
  ~MOSDPGLog() {}

public:
  const char *get_type_name() const { return "PGlog"; }
  void print(ostream& out) const {
    out << "pg_log(" << info.pgid << " epoch " << epoch
	<< " query_epoch " << query_epoch << ")";
  }

  void encode_payload(uint64_t features) {
    ::encode(epoch, payload);
    ::encode(info, payload);
    ::encode(log, payload);
    ::encode(missing, payload);
    ::encode(query_epoch, payload);
    ::encode(past_intervals, payload);
  }
  void decode_payload() {
    bufferlist::iterator p = payload.begin();
    ::decode(epoch, p);
    ::decode(info, p);
    log.decode(p, info.pgid.pool());
    missing.decode(p, info.pgid.pool());
    if (header.version >= 2) {
      ::decode(query_epoch, p);
    }
    if (header.version >= 3) {
      ::decode(past_intervals, p);
    }
  }
};

#endif
