#ifndef MINER
#define MINER

#include "Blockchain.h"
#include "CoinbaseTransaction.h"
#include "CryptoWallet.h"
#include "TransactionPool.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <thread>

class Miner {
public:
  // Initializiation with mininReward makes sense?
  // Allow updating of mining reward?
  Miner(double miningReward, EVP_PKEY &minerKeys);

  // Consider removing
  Miner(const Miner &minerToCopy);

  // Asynchronous, return success once added to pool, even before mining
  bool addTransactionToPool(const Transaction &transaction);

  /*
   * Return the number of accepted transactions
   */
  int receiveTransactionPool(const vector<Transaction> &transactions);

  bool receiveNewBlock(const Block &newBlock);

  bool receiveChain(Blockchain &&chain);

  // Return copy for safer concurrency ... Optimize?
  Block getLatestBlock();

  // Return copy for safer concurrency ... Optimize?
  Blockchain getBlockchain();

  void joinMiner();

private:
  vector<thread> workers;
  Blockchain blockChain;
  EVP_PKEY &minerKeys;
  TransactionPool transactionPool;
  atomic<int> flag{0};

  void generateBlock();

  Block mineBlock(uint32_t difficulty, uint32_t blockIndex,
                  const vector<unsigned char> &previousBlockHash);

  unique_ptr<CoinbaseTransaction>
  generateSignedCoinbaseTransaction(EVP_PKEY &minerKeys,
                                    uint32_t blockIndex) const;

  // Remove newly spent transaction outputs and add newly created unspent
  // transaction outputs
  void processNewBlock(const Block &newBlock);

  void replaceChain(Blockchain &&chainReplacingCurrent);
};

#endif
