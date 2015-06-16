/**
 * Autogenerated by Thrift Compiler (1.0.0-dev)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Migrater_H
#define Migrater_H

#include <thrift/TDispatchProcessor.h>
#include "migrater_types.h"
#include "SharedService.h"

namespace tutorial {

class MigraterIf : virtual public  ::shared::SharedServiceIf {
 public:
  virtual ~MigraterIf() {}
  virtual int32_t migrate(const int32_t num1, const std::vector<std::string> & list1) = 0;
};

class MigraterIfFactory : virtual public  ::shared::SharedServiceIfFactory {
 public:
  typedef MigraterIf Handler;

  virtual ~MigraterIfFactory() {}

  virtual MigraterIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler( ::shared::SharedServiceIf* /* handler */) = 0;
};

class MigraterIfSingletonFactory : virtual public MigraterIfFactory {
 public:
  MigraterIfSingletonFactory(const boost::shared_ptr<MigraterIf>& iface) : iface_(iface) {}
  virtual ~MigraterIfSingletonFactory() {}

  virtual MigraterIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler( ::shared::SharedServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<MigraterIf> iface_;
};

class MigraterNull : virtual public MigraterIf , virtual public  ::shared::SharedServiceNull {
 public:
  virtual ~MigraterNull() {}
  int32_t migrate(const int32_t /* num1 */, const std::vector<std::string> & /* list1 */) {
    int32_t _return = 0;
    return _return;
  }
};

typedef struct _Migrater_migrate_args__isset {
  _Migrater_migrate_args__isset() : num1(false), list1(false) {}
  bool num1;
  bool list1;
} _Migrater_migrate_args__isset;

class Migrater_migrate_args {
 public:

  static const char* ascii_fingerprint; // = "B8D94EBFFE99C6E7010418EB2DF36DC3";
  static const uint8_t binary_fingerprint[16]; // = {0xB8,0xD9,0x4E,0xBF,0xFE,0x99,0xC6,0xE7,0x01,0x04,0x18,0xEB,0x2D,0xF3,0x6D,0xC3};

  Migrater_migrate_args() : num1(0) {
  }

  virtual ~Migrater_migrate_args() throw() {}

  int32_t num1;
  std::vector<std::string>  list1;

  _Migrater_migrate_args__isset __isset;

  void __set_num1(const int32_t val) {
    num1 = val;
  }

  void __set_list1(const std::vector<std::string> & val) {
    list1 = val;
  }

  bool operator == (const Migrater_migrate_args & rhs) const
  {
    if (!(num1 == rhs.num1))
      return false;
    if (!(list1 == rhs.list1))
      return false;
    return true;
  }
  bool operator != (const Migrater_migrate_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Migrater_migrate_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Migrater_migrate_pargs {
 public:

  static const char* ascii_fingerprint; // = "B8D94EBFFE99C6E7010418EB2DF36DC3";
  static const uint8_t binary_fingerprint[16]; // = {0xB8,0xD9,0x4E,0xBF,0xFE,0x99,0xC6,0xE7,0x01,0x04,0x18,0xEB,0x2D,0xF3,0x6D,0xC3};


  virtual ~Migrater_migrate_pargs() throw() {}

  const int32_t* num1;
  const std::vector<std::string> * list1;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Migrater_migrate_result__isset {
  _Migrater_migrate_result__isset() : success(false) {}
  bool success;
} _Migrater_migrate_result__isset;

class Migrater_migrate_result {
 public:

  static const char* ascii_fingerprint; // = "32183C4A04E706C58ED2F62566DDD8DE";
  static const uint8_t binary_fingerprint[16]; // = {0x32,0x18,0x3C,0x4A,0x04,0xE7,0x06,0xC5,0x8E,0xD2,0xF6,0x25,0x66,0xDD,0xD8,0xDE};

  Migrater_migrate_result() : success(0) {
  }

  virtual ~Migrater_migrate_result() throw() {}

  int32_t success;

  _Migrater_migrate_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const Migrater_migrate_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Migrater_migrate_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Migrater_migrate_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Migrater_migrate_presult__isset {
  _Migrater_migrate_presult__isset() : success(false) {}
  bool success;
} _Migrater_migrate_presult__isset;

class Migrater_migrate_presult {
 public:

  static const char* ascii_fingerprint; // = "32183C4A04E706C58ED2F62566DDD8DE";
  static const uint8_t binary_fingerprint[16]; // = {0x32,0x18,0x3C,0x4A,0x04,0xE7,0x06,0xC5,0x8E,0xD2,0xF6,0x25,0x66,0xDD,0xD8,0xDE};


  virtual ~Migrater_migrate_presult() throw() {}

  int32_t* success;

  _Migrater_migrate_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class MigraterClient : virtual public MigraterIf, public  ::shared::SharedServiceClient {
 public:
  MigraterClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
     ::shared::SharedServiceClient(prot, prot) {}
  MigraterClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :     ::shared::SharedServiceClient(iprot, oprot) {}
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  int32_t migrate(const int32_t num1, const std::vector<std::string> & list1);
  void send_migrate(const int32_t num1, const std::vector<std::string> & list1);
  int32_t recv_migrate();
};

class MigraterProcessor : public  ::shared::SharedServiceProcessor {
 protected:
  boost::shared_ptr<MigraterIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (MigraterProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_migrate(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  MigraterProcessor(boost::shared_ptr<MigraterIf> iface) :
     ::shared::SharedServiceProcessor(iface),
    iface_(iface) {
    processMap_["migrate"] = &MigraterProcessor::process_migrate;
  }

  virtual ~MigraterProcessor() {}
};

class MigraterProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  MigraterProcessorFactory(const ::boost::shared_ptr< MigraterIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< MigraterIfFactory > handlerFactory_;
};

class MigraterMultiface : virtual public MigraterIf, public  ::shared::SharedServiceMultiface {
 public:
  MigraterMultiface(std::vector<boost::shared_ptr<MigraterIf> >& ifaces) : ifaces_(ifaces) {
    std::vector<boost::shared_ptr<MigraterIf> >::iterator iter;
    for (iter = ifaces.begin(); iter != ifaces.end(); ++iter) {
       ::shared::SharedServiceMultiface::add(*iter);
    }
  }
  virtual ~MigraterMultiface() {}
 protected:
  std::vector<boost::shared_ptr<MigraterIf> > ifaces_;
  MigraterMultiface() {}
  void add(boost::shared_ptr<MigraterIf> iface) {
     ::shared::SharedServiceMultiface::add(iface);
    ifaces_.push_back(iface);
  }
 public:
  int32_t migrate(const int32_t num1, const std::vector<std::string> & list1) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->migrate(num1, list1);
    }
    return ifaces_[i]->migrate(num1, list1);
  }

};

} // namespace

#endif