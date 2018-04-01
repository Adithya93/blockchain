#include "MinerServer.h"
#include "MinerHandler.h"
#include "NetworkConfig.h"
#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TServerSocket.h>

#define PUBLIC_KEY_FILE "default_pub.pem"
#define PRIVATE_KEY_FILE "default_priv.pem"
#define COINBASE_AMOUNT 100

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

MinerServer::MinerServer(double miningReward, EVP_PKEY &minerKeys)
    : miner(miningReward, minerKeys) {}

void MinerServer::launchServer() {
  cout << "Initializing handler" << endl;
  ::apache::thrift::stdcxx::shared_ptr<MinerHandler> handler(
      new MinerHandler(miner));
  ::apache::thrift::stdcxx::shared_ptr<TProcessor> processor(
      new MinerProcessor(handler));
  ::apache::thrift::stdcxx::shared_ptr<TServerTransport> serverTransport(
      new TServerSocket(NetworkConfig::MINER_SERVER_PORT));
  ::apache::thrift::stdcxx::shared_ptr<TTransportFactory> transportFactory(
      new TBufferedTransportFactory());
  ::apache::thrift::stdcxx::shared_ptr<TProtocolFactory> protocolFactory(
      new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory,
                       protocolFactory);
  cout << "Launching Server on port " << NetworkConfig::MINER_SERVER_PORT << "!"
       << endl;
  server.serve();
}

int main() {
  pair<EVP_PKEY &, EVP_PKEY &> minerKeyPair =
      CryptoWallet::readKeyPair(PUBLIC_KEY_FILE, PRIVATE_KEY_FILE);
  EVP_PKEY &minerPrivateKey = minerKeyPair.second;
  MinerServer minerServer(COINBASE_AMOUNT, minerPrivateKey);
  minerServer.launchServer();
}
