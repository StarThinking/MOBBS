/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "thrift/client_service/ClientService.h"
#include "thrift/monitor_service/MonitorService.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace monitor;
using namespace librbd;


int main(int argc, char** argv) {
  boost::shared_ptr<TTransport> socket(new TSocket("10.0.0.10", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  //MonitorServerClient client(protocol);
  //ClientServiceClient client(protocol);
	MonitorServiceClient client(protocol);

  try {
    transport->open();
    //client.begin_migration("a", 0, 1);
    //client.finish_migration("a", 0, 1);
		client.finish_lock("aa");
		ClientInfo ci;
		ExtentInfo ei1;
		ei1.m_eid = "abc";
		ei1.m_pool = 0;
		ExtentInfo ei2;
		ei2.m_eid = "ccc";
		ei2.m_pool = 1;
		ci.m_extents["abc"] = ei1;
		ci.m_extents["ccc"] = ei2;
		ci.m_ip = "10.0.0.10";
		client.report_client_info(ci);
    transport->close();
    /*
    cout << "get: " << argv[1] << endl;
    cout << client.lock_and_get_pool(argv[1]) << endl;
    client.release_lock(argv[1]);
    */

  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }

}
