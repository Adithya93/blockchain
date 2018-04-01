
#include "Wallet.h"
#include "networking/NetworkConfig.h"
#include "networking/protocol/SerializationUtils.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/stdcxx.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

Wallet::Wallet(const string &publicKeyFileName,
               const string &privateKeyFileName)
    : minerClient(Wallet::initializeMinerClient()) {
  CryptoWallet::initializeOpenSSL();
  if (!(ifstream(publicKeyFileName).good())) {
    // File doesn't exist yet, generate key pair and save to this filename
    generateKeyPairAndSaveToFile(publicKeyFileName, privateKeyFileName);
  } else {
    readKeyPairFromFile(publicKeyFileName, privateKeyFileName);
  }
  workers.push_back(thread{&Wallet::pollLatestBlockchain, this});
}

EVP_PKEY &Wallet::generateKeyPairAndSaveToFile(string publicKeyFileName,
                                               string privateKeyFileName) {
  EVP_PKEY &generatedKey = CryptoWallet::generateKeys(
      publicKeyFileName.c_str(), privateKeyFileName.c_str());
  activeKeys.push_back(
      // make_pair doesn't work here cos EVP_PKEY is weird
      pair<EVP_PKEY &, EVP_PKEY &>(generatedKey, generatedKey));
  return generatedKey;
}

pair<EVP_PKEY &, EVP_PKEY &>
Wallet::readKeyPairFromFile(string publicKeyFileName,
                            string privateKeyFileName) {
  pair<EVP_PKEY &, EVP_PKEY &> keyPair = CryptoWallet::readKeyPair(
      publicKeyFileName.c_str(), privateKeyFileName.c_str());
  activeKeys.push_back(keyPair);
  return keyPair;
}

// Total balance for all active (currently read into wallet) keys
double Wallet::getBalanceForWallet() {
  return accumulateT(getUnspentDataForWallet());
}

double Wallet::getBalanceForAddress(const vector<unsigned char> &address) {
  return accumulateT(getUnspentDataForAddress(address));
}

/*
 * Uses any available key that has been read into memory, returning
 * transaction hash upon submission of transaction to miner(s)
 */
string Wallet::generateTransaction(const vector<unsigned char> &fromAddress,
                                   const vector<unsigned char> &toAddress,
                                   double dataToSend) {
  for (const pair<EVP_PKEY &, EVP_PKEY &> &keyPair : activeKeys) {
    pair<vector<UnspentTransactionOutput>, double> dataSourcesForKey =
        accumulateUnspentTransactionOutputsForAddress(
            CryptoWallet::serializePublicKey(keyPair.first), dataToSend);
    if (dataSourcesForKey.second >= dataToSend) {
      return generateTransactionUsingSpecifiedKeyFromSpecifiedUnspentTransactionOutputs(
          fromAddress, toAddress, dataToSend, keyPair.second,
          dataSourcesForKey);
    }
  }
  // None of the addresses have sufficient funds
  return string(ERROR_INSUFFICIENT_FUNDS_WALLET);
}

/*
 * Variant of above that uses specified key
 */
string Wallet::generateTransactionUsingSpecifiedKey(
    const vector<unsigned char> &fromAddress,
    const vector<unsigned char> &toAddress, double dataToSend,
    EVP_PKEY &clientKey) {
  pair<vector<UnspentTransactionOutput>, double>
      dataSourcesAndTotalAvailableDataForAddress =
          accumulateUnspentTransactionOutputsForAddress(fromAddress,
                                                        dataToSend);
  if (dataSourcesAndTotalAvailableDataForAddress.second < dataToSend) {
    // Insufficient data
    return string(ERROR_INSUFFICIENT_FUNDS_ADDRESS);
  }
  return generateTransactionUsingSpecifiedKeyFromSpecifiedUnspentTransactionOutputs(
      fromAddress, toAddress, dataToSend, clientKey,
      dataSourcesAndTotalAvailableDataForAddress);
}

const vector<Transaction> &Wallet::getPendingTransactions() const {
  return pendingTransactions;
}

void Wallet::runInBackground() {
  for (auto &worker : workers) {
    worker.join();
  }
}

// NOTE : Only considers keys that have been read in
vector<double> Wallet::getUnspentDataForWallet() {
  vector<double> unspentDataForWallet;
  for (const pair<EVP_PKEY &, EVP_PKEY &> &keyPair : activeKeys) {
    vector<double> unspentDataForAddress = getUnspentDataForAddress(
        CryptoWallet::serializePublicKey(keyPair.first));
    unspentDataForWallet.insert(unspentDataForWallet.end(),
                                unspentDataForAddress.begin(),
                                unspentDataForAddress.end());
  }
  return unspentDataForWallet;
}

vector<double>
Wallet::getUnspentDataForAddress(const vector<unsigned char> &address) {
  return blockchain.getUnspentDataForAddress(address);
}

string Wallet::
    generateTransactionUsingSpecifiedKeyFromSpecifiedUnspentTransactionOutputs(
        const vector<unsigned char> &fromAddress,
        const vector<unsigned char> &toAddress, double dataToSend,
        EVP_PKEY &clientKey,
        pair<vector<UnspentTransactionOutput>, double>
            dataSourcesAndTotalAvailableDataForAddress) {
  double dataLeftOver =
      dataSourcesAndTotalAvailableDataForAddress.second - dataToSend;
  vector<TransactionInput> transactionInputs;
  for (const UnspentTransactionOutput &unspentTransactionOutput :
       dataSourcesAndTotalAvailableDataForAddress.first) {
    transactionInputs.push_back(
        TransactionInput(unspentTransactionOutput.getSourceTransactionId(),
                         unspentTransactionOutput.getSourceTransactionIndex()));
  }
  vector<TransactionOutput> transactionOutputs = {
      TransactionOutput(toAddress, dataToSend)};
  if (dataLeftOver > 0) {
    transactionOutputs.push_back(TransactionOutput(fromAddress, dataLeftOver));
  }
  Transaction transaction(transactionInputs, transactionOutputs);
  transaction.signTransactionInputs(clientKey);
  { // Synchronize concurrent access to Thrift Client (not inherently
    // thread-safe)
    lock_guard<mutex> lck(chainMutex);
    minerClient.addTransactionToPool(
        SerializationUtils::serializeTransaction(transaction));
  }
  cout << "Submitted transaction to miner!" << endl;
  pendingTransactions.push_back(transaction);
  return transaction.getHashString();
}

// TODO - try and rewrite as packaged task instead of thread with mutex, for
// better abstraction
void Wallet::pollLatestBlockchain() {
  unique_lock<mutex> lck(chainMutex);
  BlockMessage blockMessage;
  int chainLength = 0;
  while (true) {
    pollingCondition.wait_for(lck,
                              chrono::seconds(DEFAULT_POLLING_PERIOD_SECONDS));
    cout << "Polling for latest block" << endl;
    // TODO - try to avoid holding lock during the whole expensive operation
    minerClient.getLatestBlock(blockMessage);
    int blockIndex = blockMessage.blockIndex;
    if (blockIndex > chainLength) {
      // Missed block, fetch whole blockchain
      cout << "Missed a block, fetching entire blockchain" << endl;
      BlockchainMessage blockchainMessage;
      minerClient.getBlockchain(blockchainMessage);
      cout << "Fetched latest blockchain" << endl;
      blockchain =
          SerializationUtils::deserializeBlockchainMessage(blockchainMessage);
    } else if (blockIndex == chainLength) {
      cout << "Fetched latest block with hash " << blockMessage.blockHash
           << endl;
      // This is the next block
      blockchain.addBlock(
          SerializationUtils::deserializeBlockMessage(blockMessage));
    }
    chainLength = blockchain.getLength();
  }
}

pair<vector<UnspentTransactionOutput>, double>
Wallet::accumulateUnspentTransactionOutputsForAddress(
    const vector<unsigned char> &address, double target) {
  vector<UnspentTransactionOutput> unspentTransactionOutputsForAddress =
      blockchain.getUnspentTransactionOutputsForAddress(address);
  // Accumulate as many as necessary to reach required amount
  double totalDataAvailable = 0.0;
  vector<UnspentTransactionOutput> transactionOutputsToConsume;
  for (const UnspentTransactionOutput &unspentTransactionOutput :
       unspentTransactionOutputsForAddress) {
    totalDataAvailable += unspentTransactionOutput.getData();
    transactionOutputsToConsume.push_back(unspentTransactionOutput);
    if (totalDataAvailable >= target) {
      break;
    }
  }
  return make_pair(transactionOutputsToConsume, totalDataAvailable);
}

double Wallet::accumulateT(const vector<double> &TContainer) const {
  return accumulate(TContainer.begin(), TContainer.end(), 0);
}

MinerClient Wallet::initializeMinerClient() {
  shared_ptr<TTransport> socket(new TSocket(
      NetworkConfig::MINER_SERVER_HOSTNAME, NetworkConfig::MINER_SERVER_PORT));
  shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  transport->open();
  return MinerClient(protocol);
}

const string Wallet::DEFAULT_PUBLIC_KEY_FILE = "default_pub.pem";
const string Wallet::DEFAULT_PRIVATE_KEY_FILE = "default_priv.pem";

/*
int main() {
  Wallet wallet;
  wallet.runInBackground();
}
*/
