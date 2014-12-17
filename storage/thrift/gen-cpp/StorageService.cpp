/**
 * Autogenerated by Thrift Compiler (0.9.2)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "StorageService.h"

namespace storage {


StorageService_do_migration_args::~StorageService_do_migration_args() throw() {
}


uint32_t StorageService_do_migration_args::read(::apache::thrift::protocol::TProtocol* iprot) {

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
          xfer += iprot->readString(this->eid);
          this->__isset.eid = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->from);
          this->__isset.from = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->to);
          this->__isset.to = true;
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

uint32_t StorageService_do_migration_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  oprot->incrementRecursionDepth();
  xfer += oprot->writeStructBegin("StorageService_do_migration_args");

  xfer += oprot->writeFieldBegin("eid", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->eid);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("from", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->from);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("to", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32(this->to);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  oprot->decrementRecursionDepth();
  return xfer;
}


StorageService_do_migration_pargs::~StorageService_do_migration_pargs() throw() {
}


uint32_t StorageService_do_migration_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  oprot->incrementRecursionDepth();
  xfer += oprot->writeStructBegin("StorageService_do_migration_pargs");

  xfer += oprot->writeFieldBegin("eid", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString((*(this->eid)));
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("from", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32((*(this->from)));
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("to", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32((*(this->to)));
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  oprot->decrementRecursionDepth();
  return xfer;
}


StorageService_do_migration_result::~StorageService_do_migration_result() throw() {
}


uint32_t StorageService_do_migration_result::read(::apache::thrift::protocol::TProtocol* iprot) {

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
    xfer += iprot->skip(ftype);
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t StorageService_do_migration_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("StorageService_do_migration_result");

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}


StorageService_do_migration_presult::~StorageService_do_migration_presult() throw() {
}


uint32_t StorageService_do_migration_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

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
    xfer += iprot->skip(ftype);
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

void StorageServiceClient::do_migration(const std::string& eid, const int32_t from, const int32_t to)
{
  send_do_migration(eid, from, to);
  recv_do_migration();
}

void StorageServiceClient::send_do_migration(const std::string& eid, const int32_t from, const int32_t to)
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("do_migration", ::apache::thrift::protocol::T_CALL, cseqid);

  StorageService_do_migration_pargs args;
  args.eid = &eid;
  args.from = &from;
  args.to = &to;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->writeEnd();
  oprot_->getTransport()->flush();
}

void StorageServiceClient::recv_do_migration()
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
  }
  if (fname.compare("do_migration") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
  }
  StorageService_do_migration_presult result;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  return;
}

bool StorageServiceProcessor::dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext) {
  ProcessMap::iterator pfn;
  pfn = processMap_.find(fname);
  if (pfn == processMap_.end()) {
    iprot->skip(::apache::thrift::protocol::T_STRUCT);
    iprot->readMessageEnd();
    iprot->getTransport()->readEnd();
    ::apache::thrift::TApplicationException x(::apache::thrift::TApplicationException::UNKNOWN_METHOD, "Invalid method name: '"+fname+"'");
    oprot->writeMessageBegin(fname, ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->writeEnd();
    oprot->getTransport()->flush();
    return true;
  }
  (this->*(pfn->second))(seqid, iprot, oprot, callContext);
  return true;
}

void StorageServiceProcessor::process_do_migration(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext)
{
  void* ctx = NULL;
  if (this->eventHandler_.get() != NULL) {
    ctx = this->eventHandler_->getContext("StorageService.do_migration", callContext);
  }
  ::apache::thrift::TProcessorContextFreer freer(this->eventHandler_.get(), ctx, "StorageService.do_migration");

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->preRead(ctx, "StorageService.do_migration");
  }

  StorageService_do_migration_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  uint32_t bytes = iprot->getTransport()->readEnd();

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->postRead(ctx, "StorageService.do_migration", bytes);
  }

  StorageService_do_migration_result result;
  try {
    iface_->do_migration(args.eid, args.from, args.to);
  } catch (const std::exception& e) {
    if (this->eventHandler_.get() != NULL) {
      this->eventHandler_->handlerError(ctx, "StorageService.do_migration");
    }

    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("do_migration", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->writeEnd();
    oprot->getTransport()->flush();
    return;
  }

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->preWrite(ctx, "StorageService.do_migration");
  }

  oprot->writeMessageBegin("do_migration", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  bytes = oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();

  if (this->eventHandler_.get() != NULL) {
    this->eventHandler_->postWrite(ctx, "StorageService.do_migration", bytes);
  }
}

::boost::shared_ptr< ::apache::thrift::TProcessor > StorageServiceProcessorFactory::getProcessor(const ::apache::thrift::TConnectionInfo& connInfo) {
  ::apache::thrift::ReleaseHandler< StorageServiceIfFactory > cleanup(handlerFactory_);
  ::boost::shared_ptr< StorageServiceIf > handler(handlerFactory_->getHandler(connInfo), cleanup);
  ::boost::shared_ptr< ::apache::thrift::TProcessor > processor(new StorageServiceProcessor(handler));
  return processor;
}
} // namespace

