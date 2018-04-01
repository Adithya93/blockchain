
#include "Miner.h"
#include "Util.h"
#include <iostream>

// Initializiation with mininReward makes sense?
// Allow updating of mining reward?
Miner::Miner(double miningReward, EVP_PKEY &minerKeys)
    : blockChain(miningReward), minerKeys(minerKeys) {
  cout << "Initializing miner" << endl;
  workers.push_back(thread{&Miner::generateBlock, this});
}

// Consider removing
Miner::Miner(const Miner &minerToCopy)
    : blockChain(minerToCopy.blockChain.getMiningReward()),
      minerKeys(minerToCopy.minerKeys) {
  cout << "Copying miner" << endl;
  workers.push_back(thread{&Miner::generateBlock, this});
}

// Asynchronous, return success once added to pool, even before mining
bool Miner::addTransactionToPool(const Transaction &transaction) {
  if (transactionPool.addToTransactionPool(
          transaction, blockChain.getUnspentTransactionOutputs())) {
    // Thread running this method changes the value of
    // flag; the mineBlock thread reads it
    flag.fetch_add(1, memory_order_release);
    return true;
  }
  return false;
}

/*
 * Return the number of accepted transactions
 */
int Miner::receiveTransactionPool(const vector<Transaction> &transactions) {
  // Only accept transactions which don't conflict with existing ones
  return transactionPool.mergeWithTransactionPool(
      transactions, blockChain.getUnspentTransactionOutputs());
}

bool Miner::receiveNewBlock(const Block &newBlock) {
  if (blockChain.addBlock(newBlock)) {
    processNewBlock(newBlock);
    return true;
  }
  cout << "Block rejected by blockchain!" << endl;
  return false;
}

bool Miner::receiveChain(Blockchain &&chain) {
  pair<bool, unordered_map<string, UnspentTransactionOutput>> validationResult =
      chain.validateBlockchain();
  if (validationResult.first) { // Received blockchain is valid
    if (chain.getAccumulatedDifficulty() >
        blockChain.getAccumulatedDifficulty()) {
      // Received chain  has higher accumulated difficulty
      replaceChain(move(chain));
      return true;
    }
    cout << "Blockchain received does not have higher accumulated difficulty"
         << endl;
  } else {
    // Ignore invalid blockchain
    // TODO - Replace printing with logging of invalid blockchain received
    cout << "Invalid blockchain received!" << endl;
  }
  return false;
}

// Return copy for safer concurrency ... Optimize?
Block Miner::getLatestBlock() { return blockChain.getLastBlock(); }

// Return copy for safer concurrency ... Optimize?
Blockchain Miner::getBlockchain() { return blockChain; }

void Miner::joinMiner() {
  for (auto &worker : workers) {
    worker.join();
  }
}

void Miner::generateBlock() {
  cout << "Generating blocks!" << endl;
  while (true) {
    // determine difficulty, blockIndex, previousBlockHash using local chain
    uint32_t difficulty =
        blockChain.determineDifficulty(); // might mutate chain's difficulty
                                          // blockIndex;
    vector<unsigned char> previousBlockHash =
        blockChain.getLastBlock().getBlockHash();
    uint32_t blockIndex = blockChain.getLength();
    // mineBlock
    cout << "About to mine block no. " << blockIndex << endl;
    const Block minedBlock =
        mineBlock(difficulty, blockIndex, previousBlockHash);
    // add minedBlock to local Blockchain and update unspent transactions
    receiveNewBlock(minedBlock);
    // TODO (afer p2p is done) broadcast block to other miners
  }
}

Block Miner::mineBlock(uint32_t difficulty, uint32_t blockIndex,
                       const vector<unsigned char> &previousBlockHash) {
  uint32_t currentTimeStamp = Util::generateTimeStamp();
  unordered_set<Transaction, Util::StringHash> poolTransactions =
      transactionPool.getTransactionPool();
  int expected = flag.load(memory_order_relaxed);
  int actual;
  int64_t nonce = 0;
  // Add coinbase transaction at top
  unique_ptr<Transaction> coinbaseTransactionPtr =
      generateSignedCoinbaseTransaction(minerKeys, blockChain.getLength());
  vector<const Transaction *> blockTransactions{coinbaseTransactionPtr.get()};
  while (true) {
    // Avoid overhead of locking or calling getTransactionPool within every
    // iteration using the atomic flag as an indication of change in
    // transaction pool
    if ((actual = flag.load(memory_order_acquire)) != expected) {
      // New transactions were added to pool, load them
      expected = actual;
      poolTransactions = transactionPool.getTransactionPool();
      cout << "New transactions detected!" << endl;
    }
    if (!poolTransactions.empty()) {
      for (const Transaction &poolTransaction : poolTransactions) {
        blockTransactions.push_back(&poolTransaction);
      }
    }
    vector<unsigned char> hashComputedForNonce =
        Block::computeHash(blockIndex, previousBlockHash, currentTimeStamp,
                           blockTransactions, difficulty, nonce);
    if (Block::hashMatchesDifficulty(hashComputedForNonce, difficulty)) {
      cout << "Found nonce!" << endl;
      // Block allocates and handles it's own resources for transaction data
      return Block(blockIndex, hashComputedForNonce, previousBlockHash,
                   currentTimeStamp, blockTransactions, difficulty, nonce);
    }
    nonce++;
  }
  // coinBaseTransactionPtr goes out of scope here, releasing memory allocated
  // in generateSignedCoinbaseTransaction
}

unique_ptr<CoinbaseTransaction>
Miner::generateSignedCoinbaseTransaction(EVP_PKEY &minerKeys,
                                         uint32_t blockIndex) const {
  unique_ptr<CoinbaseTransaction> coinbaseTransactionPtr(
      new CoinbaseTransaction(blockIndex, minerKeys,
                              blockChain.getMiningReward()));
  return coinbaseTransactionPtr;
}

// Update transaction pool
void Miner::processNewBlock(const Block &newBlock) {
  if (newBlock.getBlockData().empty()) {
    // TODO - Log error instead of just printing
    cout << "New block has no transactions at all, ignoring" << endl;
    return;
  }
  transactionPool.updateTransactionPool(
      blockChain.getUnspentTransactionOutputs());
  cout << "Updated transaction pool!" << endl;
  flag.fetch_add(1, memory_order_release);
}

void Miner::replaceChain(Blockchain &&chainReplacingCurrent) {
  blockChain = move(chainReplacingCurrent);
  transactionPool.updateTransactionPool(
      blockChain.getUnspentTransactionOutputs());
  flag.fetch_add(1, memory_order_release);
}
