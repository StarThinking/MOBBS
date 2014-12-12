/**
 * Autogenerated by Thrift Compiler (0.9.2)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "monitor_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

namespace monitor {


ExtentInfo::~ExtentInfo() throw() {
}


void ExtentInfo::__set_m_eid(const std::string& val) {
  this->m_eid = val;
}

void ExtentInfo::__set_m_pool(const int32_t val) {
  this->m_pool = val;
}

void ExtentInfo::__set_m_rio(const int32_t val) {
  this->m_rio = val;
}

void ExtentInfo::__set_m_wio(const int32_t val) {
  this->m_wio = val;
}

const char* ExtentInfo::ascii_fingerprint = "9284B42FBE364FECA7AF8E76C1F488A2";
const uint8_t ExtentInfo::binary_fingerprint[16] = {0x92,0x84,0xB4,0x2F,0xBE,0x36,0x4F,0xEC,0xA7,0xAF,0x8E,0x76,0xC1,0xF4,0x88,0xA2};

uint32_t ExtentInfo::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->m_eid);
          this->__isset.m_eid = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->m_pool);
          this->__isset.m_pool = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->m_rio);
          this->__isset.m_rio = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->m_wio);
          this->__isset.m_wio = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t ExtentInfo::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  oprot->incrementRecursionDepth();
  xfer += oprot->writeStructBegin("ExtentInfo");

  xfer += oprot->writeFieldBegin("m_eid", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->m_eid);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("m_pool", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->m_pool);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("m_rio", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32(this->m_rio);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("m_wio", ::apache::thrift::protocol::T_I32, 4);
  xfer += oprot->writeI32(this->m_wio);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  oprot->decrementRecursionDepth();
  return xfer;
}

void swap(ExtentInfo &a, ExtentInfo &b) {
  using ::std::swap;
  swap(a.m_eid, b.m_eid);
  swap(a.m_pool, b.m_pool);
  swap(a.m_rio, b.m_rio);
  swap(a.m_wio, b.m_wio);
  swap(a.__isset, b.__isset);
}

ExtentInfo::ExtentInfo(const ExtentInfo& other0) {
  m_eid = other0.m_eid;
  m_pool = other0.m_pool;
  m_rio = other0.m_rio;
  m_wio = other0.m_wio;
  __isset = other0.__isset;
}
ExtentInfo& ExtentInfo::operator=(const ExtentInfo& other1) {
  m_eid = other1.m_eid;
  m_pool = other1.m_pool;
  m_rio = other1.m_rio;
  m_wio = other1.m_wio;
  __isset = other1.__isset;
  return *this;
}
std::ostream& operator<<(std::ostream& out, const ExtentInfo& obj) {
  using apache::thrift::to_string;
  out << "ExtentInfo(";
  out << "m_eid=" << to_string(obj.m_eid);
  out << ", " << "m_pool=" << to_string(obj.m_pool);
  out << ", " << "m_rio=" << to_string(obj.m_rio);
  out << ", " << "m_wio=" << to_string(obj.m_wio);
  out << ")";
  return out;
}


ClientInfo::~ClientInfo() throw() {
}


void ClientInfo::__set_m_extents(const std::vector<ExtentInfo> & val) {
  this->m_extents = val;
}

void ClientInfo::__set_m_ip(const std::string& val) {
  this->m_ip = val;
}

const char* ClientInfo::ascii_fingerprint = "702D09A374F9BE15C62A731AA31AFFA2";
const uint8_t ClientInfo::binary_fingerprint[16] = {0x70,0x2D,0x09,0xA3,0x74,0xF9,0xBE,0x15,0xC6,0x2A,0x73,0x1A,0xA3,0x1A,0xFF,0xA2};

uint32_t ClientInfo::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->m_extents.clear();
            uint32_t _size2;
            ::apache::thrift::protocol::TType _etype5;
            xfer += iprot->readListBegin(_etype5, _size2);
            this->m_extents.resize(_size2);
            uint32_t _i6;
            for (_i6 = 0; _i6 < _size2; ++_i6)
            {
              xfer += this->m_extents[_i6].read(iprot);
            }
            xfer += iprot->readListEnd();
          }
          this->__isset.m_extents = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->m_ip);
          this->__isset.m_ip = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t ClientInfo::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  oprot->incrementRecursionDepth();
  xfer += oprot->writeStructBegin("ClientInfo");

  xfer += oprot->writeFieldBegin("m_extents", ::apache::thrift::protocol::T_LIST, 1);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRUCT, static_cast<uint32_t>(this->m_extents.size()));
    std::vector<ExtentInfo> ::const_iterator _iter7;
    for (_iter7 = this->m_extents.begin(); _iter7 != this->m_extents.end(); ++_iter7)
    {
      xfer += (*_iter7).write(oprot);
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("m_ip", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->m_ip);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  oprot->decrementRecursionDepth();
  return xfer;
}

void swap(ClientInfo &a, ClientInfo &b) {
  using ::std::swap;
  swap(a.m_extents, b.m_extents);
  swap(a.m_ip, b.m_ip);
  swap(a.__isset, b.__isset);
}

ClientInfo::ClientInfo(const ClientInfo& other8) {
  m_extents = other8.m_extents;
  m_ip = other8.m_ip;
  __isset = other8.__isset;
}
ClientInfo& ClientInfo::operator=(const ClientInfo& other9) {
  m_extents = other9.m_extents;
  m_ip = other9.m_ip;
  __isset = other9.__isset;
  return *this;
}
std::ostream& operator<<(std::ostream& out, const ClientInfo& obj) {
  using apache::thrift::to_string;
  out << "ClientInfo(";
  out << "m_extents=" << to_string(obj.m_extents);
  out << ", " << "m_ip=" << to_string(obj.m_ip);
  out << ")";
  return out;
}

} // namespace
