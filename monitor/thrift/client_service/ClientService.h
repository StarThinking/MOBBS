/**
 * Autogenerated by Thrift Compiler (0.9.2)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef ClientService_H
#define ClientService_H

#include <thrift/TDispatchProcessor.h>
#include "client_types.h"

namespace librbd {

class ClientServiceIf {
 public:
  virtual ~ClientServiceIf() {}
  virtual void begin_migration(const std::string& eid, const int32_t from, const int32_t to) = 0;
  virtual void finish_migration(const std::string& eid, const int32_t from, const int32_t to) = 0;
};

class ClientServiceIfFactory {
 public:
  typedef ClientServiceIf Handler;

  virtual ~ClientServiceIfFactory() {}

  virtual ClientServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(ClientServiceIf* /* handler */) = 0;
};

class ClientServiceIfSingletonFactory : virtual public ClientServiceIfFactory {
 public:
  ClientServiceIfSingletonFactory(const boost::shared_ptr<ClientServiceIf>& iface) : iface_(iface) {}
  virtual ~ClientServiceIfSingletonFactory() {}

  virtual ClientServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(ClientServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<ClientServiceIf> iface_;
};

class ClientServiceNull : virtual public ClientServiceIf {
 public:
  virtual ~ClientServiceNull() {}
  void begin_migration(const std::string& /* eid */, const int32_t /* from */, const int32_t /* to */) {
    return;
  }
  void finish_migration(const std::string& /* eid */, const int32_t /* from */, const int32_t /* to */) {
    return;
  }
};

typedef struct _ClientService_begin_migration_args__isset {
  _ClientService_begin_migration_args__isset() : eid(false), from(false), to(false) {}
  bool eid :1;
  bool from :1;
  bool to :1;
} _ClientService_begin_migration_args__isset;

class ClientService_begin_migration_args {
 public:

  static const char* ascii_fingerprint; // = "28C2ECC89260BADB9C70330FBF47BFA8";
  static const uint8_t binary_fingerprint[16]; // = {0x28,0xC2,0xEC,0xC8,0x92,0x60,0xBA,0xDB,0x9C,0x70,0x33,0x0F,0xBF,0x47,0xBF,0xA8};

  ClientService_begin_migration_args(const ClientService_begin_migration_args&);
  ClientService_begin_migration_args& operator=(const ClientService_begin_migration_args&);
  ClientService_begin_migration_args() : eid(), from(0), to(0) {
  }

  virtual ~ClientService_begin_migration_args() throw();
  std::string eid;
  int32_t from;
  int32_t to;

  _ClientService_begin_migration_args__isset __isset;

  void __set_eid(const std::string& val);

  void __set_from(const int32_t val);

  void __set_to(const int32_t val);

  bool operator == (const ClientService_begin_migration_args & rhs) const
  {
    if (!(eid == rhs.eid))
      return false;
    if (!(from == rhs.from))
      return false;
    if (!(to == rhs.to))
      return false;
    return true;
  }
  bool operator != (const ClientService_begin_migration_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientService_begin_migration_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const ClientService_begin_migration_args& obj);
};


class ClientService_begin_migration_pargs {
 public:

  static const char* ascii_fingerprint; // = "28C2ECC89260BADB9C70330FBF47BFA8";
  static const uint8_t binary_fingerprint[16]; // = {0x28,0xC2,0xEC,0xC8,0x92,0x60,0xBA,0xDB,0x9C,0x70,0x33,0x0F,0xBF,0x47,0xBF,0xA8};


  virtual ~ClientService_begin_migration_pargs() throw();
  const std::string* eid;
  const int32_t* from;
  const int32_t* to;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const ClientService_begin_migration_pargs& obj);
};


class ClientService_begin_migration_result {
 public:

  static const char* ascii_fingerprint; // = "99914B932BD37A50B983C5E7C90AE93B";
  static const uint8_t binary_fingerprint[16]; // = {0x99,0x91,0x4B,0x93,0x2B,0xD3,0x7A,0x50,0xB9,0x83,0xC5,0xE7,0xC9,0x0A,0xE9,0x3B};

  ClientService_begin_migration_result(const ClientService_begin_migration_result&);
  ClientService_begin_migration_result& operator=(const ClientService_begin_migration_result&);
  ClientService_begin_migration_result() {
  }

  virtual ~ClientService_begin_migration_result() throw();

  bool operator == (const ClientService_begin_migration_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const ClientService_begin_migration_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientService_begin_migration_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const ClientService_begin_migration_result& obj);
};


class ClientService_begin_migration_presult {
 public:

  static const char* ascii_fingerprint; // = "99914B932BD37A50B983C5E7C90AE93B";
  static const uint8_t binary_fingerprint[16]; // = {0x99,0x91,0x4B,0x93,0x2B,0xD3,0x7A,0x50,0xB9,0x83,0xC5,0xE7,0xC9,0x0A,0xE9,0x3B};


  virtual ~ClientService_begin_migration_presult() throw();

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

  friend std::ostream& operator<<(std::ostream& out, const ClientService_begin_migration_presult& obj);
};

typedef struct _ClientService_finish_migration_args__isset {
  _ClientService_finish_migration_args__isset() : eid(false), from(false), to(false) {}
  bool eid :1;
  bool from :1;
  bool to :1;
} _ClientService_finish_migration_args__isset;

class ClientService_finish_migration_args {
 public:

  static const char* ascii_fingerprint; // = "28C2ECC89260BADB9C70330FBF47BFA8";
  static const uint8_t binary_fingerprint[16]; // = {0x28,0xC2,0xEC,0xC8,0x92,0x60,0xBA,0xDB,0x9C,0x70,0x33,0x0F,0xBF,0x47,0xBF,0xA8};

  ClientService_finish_migration_args(const ClientService_finish_migration_args&);
  ClientService_finish_migration_args& operator=(const ClientService_finish_migration_args&);
  ClientService_finish_migration_args() : eid(), from(0), to(0) {
  }

  virtual ~ClientService_finish_migration_args() throw();
  std::string eid;
  int32_t from;
  int32_t to;

  _ClientService_finish_migration_args__isset __isset;

  void __set_eid(const std::string& val);

  void __set_from(const int32_t val);

  void __set_to(const int32_t val);

  bool operator == (const ClientService_finish_migration_args & rhs) const
  {
    if (!(eid == rhs.eid))
      return false;
    if (!(from == rhs.from))
      return false;
    if (!(to == rhs.to))
      return false;
    return true;
  }
  bool operator != (const ClientService_finish_migration_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientService_finish_migration_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const ClientService_finish_migration_args& obj);
};


class ClientService_finish_migration_pargs {
 public:

  static const char* ascii_fingerprint; // = "28C2ECC89260BADB9C70330FBF47BFA8";
  static const uint8_t binary_fingerprint[16]; // = {0x28,0xC2,0xEC,0xC8,0x92,0x60,0xBA,0xDB,0x9C,0x70,0x33,0x0F,0xBF,0x47,0xBF,0xA8};


  virtual ~ClientService_finish_migration_pargs() throw();
  const std::string* eid;
  const int32_t* from;
  const int32_t* to;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const ClientService_finish_migration_pargs& obj);
};


class ClientService_finish_migration_result {
 public:

  static const char* ascii_fingerprint; // = "99914B932BD37A50B983C5E7C90AE93B";
  static const uint8_t binary_fingerprint[16]; // = {0x99,0x91,0x4B,0x93,0x2B,0xD3,0x7A,0x50,0xB9,0x83,0xC5,0xE7,0xC9,0x0A,0xE9,0x3B};

  ClientService_finish_migration_result(const ClientService_finish_migration_result&);
  ClientService_finish_migration_result& operator=(const ClientService_finish_migration_result&);
  ClientService_finish_migration_result() {
  }

  virtual ~ClientService_finish_migration_result() throw();

  bool operator == (const ClientService_finish_migration_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const ClientService_finish_migration_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientService_finish_migration_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const ClientService_finish_migration_result& obj);
};


class ClientService_finish_migration_presult {
 public:

  static const char* ascii_fingerprint; // = "99914B932BD37A50B983C5E7C90AE93B";
  static const uint8_t binary_fingerprint[16]; // = {0x99,0x91,0x4B,0x93,0x2B,0xD3,0x7A,0x50,0xB9,0x83,0xC5,0xE7,0xC9,0x0A,0xE9,0x3B};


  virtual ~ClientService_finish_migration_presult() throw();

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

  friend std::ostream& operator<<(std::ostream& out, const ClientService_finish_migration_presult& obj);
};

class ClientServiceClient : virtual public ClientServiceIf {
 public:
  ClientServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  ClientServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void begin_migration(const std::string& eid, const int32_t from, const int32_t to);
  void send_begin_migration(const std::string& eid, const int32_t from, const int32_t to);
  void recv_begin_migration();
  void finish_migration(const std::string& eid, const int32_t from, const int32_t to);
  void send_finish_migration(const std::string& eid, const int32_t from, const int32_t to);
  void recv_finish_migration();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class ClientServiceProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<ClientServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (ClientServiceProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_begin_migration(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_finish_migration(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  ClientServiceProcessor(boost::shared_ptr<ClientServiceIf> iface) :
    iface_(iface) {
    processMap_["begin_migration"] = &ClientServiceProcessor::process_begin_migration;
    processMap_["finish_migration"] = &ClientServiceProcessor::process_finish_migration;
  }

  virtual ~ClientServiceProcessor() {}
};

class ClientServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  ClientServiceProcessorFactory(const ::boost::shared_ptr< ClientServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< ClientServiceIfFactory > handlerFactory_;
};

class ClientServiceMultiface : virtual public ClientServiceIf {
 public:
  ClientServiceMultiface(std::vector<boost::shared_ptr<ClientServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~ClientServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<ClientServiceIf> > ifaces_;
  ClientServiceMultiface() {}
  void add(boost::shared_ptr<ClientServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void begin_migration(const std::string& eid, const int32_t from, const int32_t to) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->begin_migration(eid, from, to);
    }
    ifaces_[i]->begin_migration(eid, from, to);
  }

  void finish_migration(const std::string& eid, const int32_t from, const int32_t to) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->finish_migration(eid, from, to);
    }
    ifaces_[i]->finish_migration(eid, from, to);
  }

};

} // namespace

#endif