#ifndef BLOCKCHAIN
#define BLOCKCHAIN

#include "Block.h"
#include "GenesisBlock.h"
#include "TransactionPool.h"
#include <thread>
#include <vector>

using namespace std;

#define DEFAULT_MINING_REWARD 100
#define STARTING_DIFFICULTY 2
// Recalibrate difficulty every 10 blocks
#define DIFFICULTY_ADJUSTMENT_INTERVAL_BLOCKS 10
// Target time in seconds for every new block - TODO : make this dynamically
// adjustable
#define BLOCK_GENERATION_INTERVAL_SECONDS 30
#define BLOCK_GENERATION_INTERVAL_DRIFT_FACTOR 2

class Blockchain {
public:
  // Genesis block added to vector upon initialization
  Blockchain(double miningReward);

  Blockchain();

  Blockchain(const Blockchain &blockChainToCopy);

  Blockchain(double miningReward, uint32_t difficulty,
             const vector<const Block *> blocks);

  Blockchain &operator=(Blockchain blockChainToCopy);

  friend void swap(Blockchain &first, Blockchain &second);

  // Perform checks using given map before adding (faster)
  bool addBlock(const Block &newBlock);

  Block getLastBlock() const;

  size_t getLength() const;

  // Maybe modified? Consider adding a updateReward method?
  double getMiningReward() const;

  uint32_t getCurrentDifficulty() const;

  uint32_t determineDifficulty();

  uint64_t getAccumulatedDifficulty() const;

  // Make copy instead of returning const ref to aid thread-safety
  vector<unique_ptr<const Block>> getChainCopy() const;

  // Make copy instead of returning const ref to aid thread-safety
  unordered_map<string, UnspentTransactionOutput>
  getUnspentTransactionOutputs() const;

  pair<bool, unordered_map<string, UnspentTransactionOutput>>
  validateBlockchain() const;

  vector<UnspentTransactionOutput> getUnspentTransactionOutputsForAddress(
      const vector<unsigned char> &address) const;

  vector<double>
  getUnspentDataForAddress(const vector<unsigned char> &address) const;

private:
  double miningReward;
  uint32_t difficulty;
  vector<unique_ptr<const Block>> chain;
  unordered_map<string, UnspentTransactionOutput> unspentTransactionOutputs;
  // TEMP Synchronize potentially concurrent writes / reads of blockChain
  mutable mutex chainMutex;
  // Synchronize potentially concurrent updates of unspentTransactionOutputs map
  mutable mutex unspentTransactionOutputsMutex;

  unique_ptr<GenesisBlock> generateGenesisBlock() const;

  bool shouldRecalibrateDifficulty() const;

  virtual bool blockHasCorrectCoinbaseData(const Block &block) const;

  template <typename R>
  vector<R> queryUnspentTransactionOutputsForAddress(
      const vector<unsigned char> &address,
      R (*queryFunction)(const UnspentTransactionOutput &)) const;

  const Block &getBlockAtIndex(int index) const;

  const Block &getFirstBlock() const;

  bool isEmpty() const;

  void addBlockToChain(const Block &block);

  static void updateTransactionOutputsForNewBlock(
      const Block &newBlock, unordered_map<string, UnspentTransactionOutput>
                                 &unspentTransactionOutputsToUpdate);

  static void removeNewlySpentTransactionOutputs(
      const vector<unique_ptr<const Transaction>> &newTransactions,
      unordered_map<string, UnspentTransactionOutput>
          &unspentTransactionOutputsToUpdate);

  static void addNewlyUnspentTransactionOutputs(
      const vector<unique_ptr<const Transaction>> &newTransactions,
      unordered_map<string, UnspentTransactionOutput>
          &unspentTransactionOutputsToUpdate);
};

#endif
