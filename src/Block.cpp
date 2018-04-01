
#include "Block.h"
#include "Crypto.h"
#include "Util.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>

Block::Block(uint32_t blockIndex, const vector<unsigned char> &blockHash,
             const vector<unsigned char> &previousBlockHash, uint64_t timestamp,
             const vector<const Transaction *> &transactionPtrs,
             uint32_t difficulty, int64_t nonce)
    : blockIndex(blockIndex), blockHash(blockHash),
      previousBlockHash(previousBlockHash), timestamp(timestamp),
      difficulty(difficulty), nonce(nonce) {
  initializeDataBasedOnIndex(blockIndex, transactionPtrs);
}

Block::Block(const Block &blockToCopy)
    : blockIndex(blockToCopy.getBlockIndex()),
      blockHash(blockToCopy.getBlockHash()),
      previousBlockHash(blockToCopy.getPreviousBlockHash()),
      timestamp(blockToCopy.getBlockTimestamp()),
      difficulty(blockToCopy.getDifficulty()), nonce(blockToCopy.getNonce()) {
  initializeDataBasedOnIndex(blockIndex,
                             getRawTransactionPtrs(blockToCopy.getBlockData()));
}

Block &Block::operator=(Block blockToCopy) {
  swap(*this, blockToCopy);
  return *this;
}

void swap(Block &first, Block &second) {
  // enable ADL
  using std::swap;

  swap(first.blockIndex, second.blockIndex);
  swap(first.blockHash, second.blockHash);
  swap(first.previousBlockHash, second.previousBlockHash);
  swap(first.timestamp, second.timestamp);
  swap(first.data, second.data);
  swap(first.difficulty, second.difficulty);
  swap(first.nonce, second.nonce);
}

// For overriding
unique_ptr<Block> Block::copyBlock() const {
  return unique_ptr<Block>(new Block(*this));
}

uint32_t Block::getBlockIndex() const { return blockIndex; }

const vector<unsigned char> &Block::getBlockHash() const { return blockHash; }

const vector<unsigned char> &Block::getPreviousBlockHash() const {
  return previousBlockHash;
}

const uint64_t &Block::getBlockTimestamp() const { return timestamp; }

// Return copy instead of actual vector of unique_ptrs for abstraction
// Causes slicing, not viable ... REDO
const vector<unique_ptr<const Transaction>> &Block::getBlockData() const {
  return data;
}

uint32_t Block::getDifficulty() const { return difficulty; }

int64_t Block::getNonce() const { return nonce; }

vector<unsigned char>
Block::computeHash(uint32_t blockIndex,
                   const vector<unsigned char> &previousBlockHash,
                   uint64_t timestamp,
                   const vector<const Transaction *> &transactionUniquePtrs,
                   uint32_t difficulty, int64_t nonce) {
  string transactionsString =
      transactionUniquePtrs.empty()
          ? serializeTransactionsData(transactionUniquePtrs)
          : string();
  return Crypto::getSHA256Hash(
      (stringstream() << blockIndex
                      << Util::stringFromUnsignedCharVec(previousBlockHash)
                      << timestamp << transactionsString << difficulty << nonce)
          .str());
}

bool Block::hashTallies() const {
  return isSameHash(computeHash(blockIndex, previousBlockHash, timestamp,
                                getRawTransactionPtrs(data), difficulty, nonce),
                    blockHash);
}

// Holistic check performed by receiving nodes (other than miner of block) to
// account for races between miners that lead to forks
// overridden by GenesisBlock
bool Block::isValidNewBlock(
    const Block &previousBlock,
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) const {
  // TEMP for debugging
  if (!fulfillsConstructionInvariant()) {
    cout << "Block is not internally consistent" << endl;
    return false;
  }
  if (!previousIndexMatches(previousBlock)) {
    cout << "Block index does not match previous index" << endl;
    return false;
  }
  if (!previousHashMatches(previousBlock)) {
    cout << "Block's previous hash does not match hash of previous block"
         << endl;
    return false;
  }
  if (!hasValidTimeStamp(previousBlock)) {
    cout << "Block timestamp invalid" << endl;
    return false;
  }
  // Check transactions of block
  return all_of(data.begin(), data.end(),
                [&](const unique_ptr<const Transaction> &blockTransactionPtr) {
                  return blockTransactionPtr->isValidTransaction(
                      unspentTransactionOutputs);
                });
}

bool Block::hasCorrectCoinbaseData(double expectedCoinbaseData) const {
  const Transaction *coinbaseTransaction = data.front().get();
  return coinbaseTransaction->getOutputs().front().getData() ==
             expectedCoinbaseData &&
         coinbaseTransaction->getInputs().front().getSourceTransactionIndex() ==
             blockIndex;
}

bool Block::hashMatchesDifficulty(const vector<unsigned char> &hash,
                                  int difficulty) {
  string hashString = Util::stringFromUnsignedCharVec(hash);
  return count(hashString.begin(), hashString.begin() + difficulty,
               ZERO_CHAR) == difficulty;
}

string Block::getHashString() const {
  return Util::stringFromUnsignedCharVec(blockHash);
}

bool Block::fulfillsConstructionInvariant() const {
  return hashTallies() && hashMatchesDifficulty(blockHash, difficulty);
}

bool Block::isSameHash(const vector<unsigned char> &hash,
                       const vector<unsigned char> &otherHash) const {
  return Util::stringFromUnsignedCharVec(hash) ==
         Util::stringFromUnsignedCharVec(otherHash);
}

bool Block::previousHashMatches(const Block &previousBlock) const {
  return isSameHash(previousBlockHash, previousBlock.getBlockHash());
}

bool Block::previousIndexMatches(const Block &previousBlock) const {
  return blockIndex == previousBlock.getBlockIndex() + 1;
}

bool Block::hasValidTimeStamp(const Block &previousBlock) const {
  return ((timestamp >
           previousBlock.getBlockTimestamp() - TIMESTAMP_DRIFT_SECONDS) &&
          (timestamp < Util::generateTimeStamp() + TIMESTAMP_DRIFT_SECONDS));
}

void Block::initializeDataBasedOnIndex(
    uint32_t blockIndex, const vector<const Transaction *> &transactionPtrs) {
  assert(blockIndex >= 0);
  if (blockIndex > 0) {
    // As all non-genesis blocks should cointain at least the
    // CoinbaseTransaction, no non-genesis block should be empty
    assert(!transactionPtrs.empty());
    initializeData(transactionPtrs);
  }
  assert(fulfillsConstructionInvariant());
}

void Block::initializeData(const vector<const Transaction *> &transactionPtrs) {
  for (const Transaction *transactionPtr : transactionPtrs) {
    data.push_back(transactionPtr->copyTransaction());
  }
}

const string Block::serializeTransactionsData(
    const vector<const Transaction *> &transactionPtrs) {
  stringstream ss;
  for (const Transaction *transactionPtr : transactionPtrs) {
    ss << transactionPtr->getHashString();
  }
  return ss.str();
}

vector<const Transaction *> Block::getRawTransactionPtrs(
    const vector<unique_ptr<const Transaction>> &transactionUniquePtrs) {
  vector<const Transaction *> rawTransactionPtrs;
  for (const unique_ptr<const Transaction> &transactionUniquePtr :
       transactionUniquePtrs) {
    rawTransactionPtrs.push_back(transactionUniquePtr.get());
  }
  return rawTransactionPtrs;
}
