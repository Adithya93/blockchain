#ifndef WALLET
#define WALLET

#include "Blockchain.h"
#include "CryptoWallet.h"
#include "networking/protocol/gen-cpp/Miner.h"
#include <condition_variable>
#include <thread>
#include <vector>

#define ERROR_INSUFFICIENT_FUNDS_ADDRESS                                       \
  "Insufficient funds for transaction in given address"
#define ERROR_INSUFFICIENT_FUNDS_WALLET                                        \
  "Insufficient funds for transaction in wallet"
#define DEFAULT_POLLING_PERIOD_SECONDS 10 // TODO allow adjustment

using namespace std;
// TODO - How to make this polymorphic? Currently only makes sense for numeric
// types ... NOTE - Possible solution : decouple 'data' from 'fee', where fee is
// numeric and data is parametrized (like now)

class Wallet {
public:
  Wallet(const string &publicKeyFileName = DEFAULT_PUBLIC_KEY_FILE,
         const string &privateKeyFileName = DEFAULT_PRIVATE_KEY_FILE);

  EVP_PKEY &generateKeyPairAndSaveToFile(string publicKeyFileName,
                                         string privateKeyFileName);

  pair<EVP_PKEY &, EVP_PKEY &> readKeyPairFromFile(string publicKeyFileName,
                                                   string privateKeyFileName);

  // Total balance for all active (currently read into wallet) keys
  double getBalanceForWallet();

  double getBalanceForAddress(const vector<unsigned char> &address);

  /*
   * Uses any available key that has been read into memory, returning
   * transaction hash upon submission of transaction to miner(s)
   */
  string generateTransaction(const vector<unsigned char> &fromAddress,
                             const vector<unsigned char> &toAddress,
                             double dataToSend);

  /*
   * Variant of above that uses specified key
   */
  string
  generateTransactionUsingSpecifiedKey(const vector<unsigned char> &fromAddress,
                                       const vector<unsigned char> &toAddress,
                                       double dataToSend, EVP_PKEY &clientKey);

  const vector<Transaction> &getPendingTransactions() const;

  void runInBackground();

private:
  vector<pair<EVP_PKEY &, EVP_PKEY &>> activeKeys;
  Blockchain blockchain;                   // will be periodically updated
  vector<Transaction> pendingTransactions; // submitted but not on ledger yet
  MinerClient minerClient;
  // Could consider caching account balances, i.e. unspent data for certain
  // addresses
  mutex chainMutex;
  // TEMP
  vector<thread> workers;
  // TEMP
  condition_variable pollingCondition;
  static const string DEFAULT_PUBLIC_KEY_FILE;
  static const string DEFAULT_PRIVATE_KEY_FILE;

  // NOTE : Only considers keys that have been read in
  vector<double> getUnspentDataForWallet();

  vector<double> getUnspentDataForAddress(const vector<unsigned char> &address);

  string
  generateTransactionUsingSpecifiedKeyFromSpecifiedUnspentTransactionOutputs(
      const vector<unsigned char> &fromAddress,
      const vector<unsigned char> &toAddress, double dataToSend,
      EVP_PKEY &clientKey,
      pair<vector<UnspentTransactionOutput>, double>
          dataSourcesAndTotalAvailableDataForAddress);

  // TODO - try and rewrite as packaged task instead of thread with mutex, for
  // better abstraction
  void pollLatestBlockchain();

  pair<vector<UnspentTransactionOutput>, double>
  accumulateUnspentTransactionOutputsForAddress(
      const vector<unsigned char> &address, double target);

  double accumulateT(const vector<double> &TContainer) const;

  Blockchain copyCurrentBlockChain();

  MinerClient initializeMinerClient();
};

#endif
