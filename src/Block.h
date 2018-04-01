#ifndef BLOCK
#define BLOCK

#include "Serializable.h"
#include "Transaction.h"
#include <vector>

#define ZERO_CHAR '0'
#define TIMESTAMP_DRIFT_SECONDS 60

using namespace std;

class Block : public Serializable {
public:
  Block(uint32_t blockIndex, const vector<unsigned char> &blockHash,
        const vector<unsigned char> &previousBlockHash, uint64_t timestamp,
        const vector<const Transaction *> &transactionPtrs, uint32_t difficulty,
        int64_t nonce);

  Block(const Block &blockToCopy);

  Block &operator=(Block blockToCopy);

  friend void swap(Block &first, Block &second);

  // For overriding
  virtual unique_ptr<Block> copyBlock() const;

  uint32_t getBlockIndex() const;

  const vector<unsigned char> &getBlockHash() const;

  const vector<unsigned char> &getPreviousBlockHash() const;

  const uint64_t &getBlockTimestamp() const;

  // Return copy instead of actual vector of unique_ptrs for abstraction
  // Causes slicing, not viable ... REDO
  const vector<unique_ptr<const Transaction>> &getBlockData() const;

  uint32_t getDifficulty() const;

  int64_t getNonce() const;

  static vector<unsigned char>
  computeHash(uint32_t blockIndex,
              const vector<unsigned char> &previousBlockHash,
              uint64_t timestamp,
              const vector<const Transaction *> &transactionUniquePtrs,
              uint32_t difficulty, int64_t nonce);

  bool hashTallies() const;

  // Holistic check performed by receiving nodes (other than miner of block) to
  // account for races between miners that lead to forks
  // overridden by GenesisBlock
  virtual bool
  isValidNewBlock(const Block &previousBlock,
                  const unordered_map<string, UnspentTransactionOutput>
                      &unspentTransactionOutputs) const;

  virtual bool hasCorrectCoinbaseData(double expectedCoinbaseData) const;

  static bool hashMatchesDifficulty(const vector<unsigned char> &hash,
                                    int difficulty);

  string getHashString() const override;

protected:
  bool fulfillsConstructionInvariant() const;

private:
  uint32_t blockIndex;
  vector<unsigned char> blockHash;
  vector<unsigned char> previousBlockHash;
  uint64_t timestamp;
  vector<unique_ptr<const Transaction>> data;
  uint32_t difficulty;
  int64_t nonce;

  bool isSameHash(const vector<unsigned char> &hash,
                         const vector<unsigned char> &otherHash) const;

  bool previousHashMatches(const Block &previousBlock) const;

  bool previousIndexMatches(const Block &previousBlock) const;

  bool hasValidTimeStamp(const Block &previousBlock) const;

  void initializeDataBasedOnIndex(
      uint32_t blockIndex, const vector<const Transaction *> &transactionPtrs);

  void initializeData(const vector<const Transaction *> &transactionPtrs);

  static const string
  serializeTransactionsData(const vector<const Transaction *> &transactionPtrs);

  static vector<const Transaction *> getRawTransactionPtrs(
      const vector<unique_ptr<const Transaction>> &transactionUniquePtrs);
};

#endif
