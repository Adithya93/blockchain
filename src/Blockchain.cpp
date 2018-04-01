#include "Blockchain.h"
#include <cassert>
#include <iostream>

// Genesis block added to vector upon initialization
Blockchain::Blockchain(double miningReward)
    : miningReward(miningReward), difficulty(STARTING_DIFFICULTY) {
  // Omitting synchronization as this is constructor, object doesn't exist yet
  chain.push_back(generateGenesisBlock());
}

Blockchain::Blockchain()
    : miningReward(DEFAULT_MINING_REWARD), difficulty(STARTING_DIFFICULTY) {
  // Omitting synchronization as this is constructor, object doesn't exist yet
  chain.push_back(generateGenesisBlock());
}

Blockchain::Blockchain(const Blockchain &blockChainToCopy)
    : miningReward(blockChainToCopy.getMiningReward()),
      difficulty(blockChainToCopy.difficulty) {
  vector<unique_ptr<const Block>> blockChainCopy =
      blockChainToCopy.getChainCopy();
  for (const unique_ptr<const Block> &blockPtrToCopy : blockChainCopy) {
    assert(addBlock(*blockPtrToCopy));
  }
}

Blockchain::Blockchain(double miningReward, uint32_t difficulty,
                       const vector<const Block *> blocks)
    : miningReward(miningReward), difficulty(difficulty) {
  for (const Block *blockPtr : blocks) {
    assert(addBlock(*blockPtr));
  }
}

Blockchain &Blockchain::operator=(Blockchain blockChainToCopy) {
  lock_guard<mutex> chainMutexLck(chainMutex);
  lock_guard<mutex> transactionOutputsMutexLck(unspentTransactionOutputsMutex);
  // Don't care about synchronizing the rvalue
  swap(*this, blockChainToCopy);
  return *this;
}

void swap(Blockchain &first, Blockchain &second) {
  using std::swap;
  swap(first.miningReward, second.miningReward);
  swap(first.difficulty, second.difficulty);
  swap(first.chain, second.chain);
}

// Perform checks before adding
bool Blockchain::addBlock(const Block &newBlock) {
  // Check if coinbase transaction has correct Coinbase Reward and index
  lock_guard<mutex> transactionOutputsMutexLck(unspentTransactionOutputsMutex);
  if (!newBlock.isValidNewBlock(isEmpty() ? newBlock : getLastBlock(),
                                unspentTransactionOutputs)) {
    cout << "Invalid new block" << endl;
    return false;
  }
  if (!blockHasCorrectCoinbaseData(newBlock)) {
    cout << "Invalid coinbase data for new block" << endl;
    return false;
  }
  addBlockToChain(newBlock);
  updateTransactionOutputsForNewBlock(newBlock, unspentTransactionOutputs);
  return true;
}

Block Blockchain::getLastBlock() const {
  lock_guard<mutex> chainMutexLck(chainMutex);
  return *(chain.back());
}

size_t Blockchain::getLength() const {
  lock_guard<mutex> chainMutexLck(chainMutex);
  return chain.size();
}

// Maybe modified? Consider adding a updateReward method?
double Blockchain::getMiningReward() const { return miningReward; }

uint32_t Blockchain::getCurrentDifficulty() const { return difficulty; }

uint32_t Blockchain::determineDifficulty() {
  if (!shouldRecalibrateDifficulty()) {
    return difficulty;
  }
  int chainLength = getLength();
  const Block &blockAtPreviousCalibration =
      getBlockAtIndex(chainLength - DIFFICULTY_ADJUSTMENT_INTERVAL_BLOCKS);
  uint64_t expectedTimeTaken =
      DIFFICULTY_ADJUSTMENT_INTERVAL_BLOCKS * BLOCK_GENERATION_INTERVAL_SECONDS;
  uint64_t actualTimeTaken = getLastBlock().getBlockTimestamp() -
                             blockAtPreviousCalibration.getBlockTimestamp();
  // Too fast, raise difficulty
  if (actualTimeTaken <
      expectedTimeTaken / BLOCK_GENERATION_INTERVAL_DRIFT_FACTOR) {
    cout << "Incrementing difficulty!" << endl;
    return ++difficulty;
  } else if (actualTimeTaken >
             expectedTimeTaken * BLOCK_GENERATION_INTERVAL_DRIFT_FACTOR) {
    // Too slow, lower difficulty
    cout << "Decrementing difficulty!" << endl;
    return --difficulty;
  }
  return difficulty;
}

uint64_t Blockchain::getAccumulatedDifficulty() const {
  uint64_t accumulatedDifficulty = 0;
  lock_guard<mutex> chainMutexLck(chainMutex);
  for_each(chain.begin(), chain.end(),
           [&](const unique_ptr<const Block> &blockPtr) {
             accumulatedDifficulty += (blockPtr->getDifficulty() << 1);
           });
  return accumulatedDifficulty;
}

vector<unique_ptr<const Block>> Blockchain::getChainCopy() const {
  vector<unique_ptr<const Block>> chainCopy;
  lock_guard<mutex> chainMutexLck(chainMutex);
  for (const unique_ptr<const Block> &blockPtr : chain) {
    chainCopy.push_back(blockPtr->copyBlock());
  }
  return chainCopy;
}

// Copying for safer concurrency
unordered_map<string, UnspentTransactionOutput>
Blockchain::getUnspentTransactionOutputs() const {
  lock_guard<mutex> lck(unspentTransactionOutputsMutex);
  return unspentTransactionOutputs;
}

pair<bool, unordered_map<string, UnspentTransactionOutput>>
Blockchain::validateBlockchain() const {
  bool validity = false;
  unordered_map<string, UnspentTransactionOutput>
      reconstructedTransactionOutputs;
  if (!isEmpty()) {
    const Block *previousBlock = &getFirstBlock();
    for (int blockIndex = 0; blockIndex < getLength(); blockIndex++) {
      const Block *currentBlock = &getBlockAtIndex(blockIndex);
      if (!currentBlock->isValidNewBlock(*previousBlock,
                                         reconstructedTransactionOutputs)) {
        cout << "Invalid block detected at index " << blockIndex << endl;
        break;
      }
      previousBlock = currentBlock;
      if (blockIndex > 0) {
        // Constructor invariant of block ensures data is non-empty, so not
        // checking here
        updateTransactionOutputsForNewBlock(*currentBlock,
                                            reconstructedTransactionOutputs);
      }
    }
    validity = true;
  }
  return make_pair(validity, reconstructedTransactionOutputs);
}

vector<UnspentTransactionOutput>
Blockchain::getUnspentTransactionOutputsForAddress(
    const vector<unsigned char> &address) const {
  return queryUnspentTransactionOutputsForAddress<UnspentTransactionOutput>(
      address, [](const UnspentTransactionOutput &unspentTransactionOutput) {
        return unspentTransactionOutput;
      });
}

vector<double> Blockchain::getUnspentDataForAddress(
    const vector<unsigned char> &address) const {
  return queryUnspentTransactionOutputsForAddress<double>(
      address, [](const UnspentTransactionOutput &unspentTransactionOutput) {
        return unspentTransactionOutput.getData();
      });
}

unique_ptr<GenesisBlock> Blockchain::generateGenesisBlock() const {
  return unique_ptr<GenesisBlock>(new GenesisBlock());
}

bool Blockchain::shouldRecalibrateDifficulty() const {
  // Account for first block being Genesis block that isn't actually mined
  size_t chainLength = getLength();
  return (chainLength > DIFFICULTY_ADJUSTMENT_INTERVAL_BLOCKS) &&
         (chainLength % DIFFICULTY_ADJUSTMENT_INTERVAL_BLOCKS == 1);
}

bool Blockchain::blockHasCorrectCoinbaseData(const Block &block) const {
  return block.hasCorrectCoinbaseData(miningReward);
}

template <typename R>
vector<R> Blockchain::queryUnspentTransactionOutputsForAddress(
    const vector<unsigned char> &address,
    R (*queryFunction)(const UnspentTransactionOutput &)) const {
  lock_guard<mutex> lck(unspentTransactionOutputsMutex);
  vector<R> unspentDataForAddress;
  for (auto &entry : unspentTransactionOutputs) {
    const UnspentTransactionOutput &unspentTransactionOutput = entry.second;
    if (unspentTransactionOutput.getSourceTransactionAddress() == address) {
      unspentDataForAddress.push_back(queryFunction(unspentTransactionOutput));
    }
  }
  return unspentDataForAddress;
}

// Private method, range-check omitted since callsites are local and trusted
const Block &Blockchain::getBlockAtIndex(int index) const {
  lock_guard<mutex> chainMutexLck(chainMutex);
  return *chain.at(index);
}

const Block &Blockchain::getFirstBlock() const {
  lock_guard<mutex> chainMutexLck(chainMutex);
  return *chain.front();
}

bool Blockchain::isEmpty() const {
  lock_guard<mutex> chainMutexLck(chainMutex);
  return chain.empty();
}

void Blockchain::addBlockToChain(const Block &block) {
  lock_guard<mutex> chainMutexLck(chainMutex);
  chain.push_back(block.copyBlock());
}

void Blockchain::updateTransactionOutputsForNewBlock(
    const Block &newBlock, unordered_map<string, UnspentTransactionOutput>
                               &unspentTransactionOutputsToUpdate) {
  const vector<unique_ptr<const Transaction>> &blockTransactions =
      newBlock.getBlockData();
  removeNewlySpentTransactionOutputs(blockTransactions,
                                     unspentTransactionOutputsToUpdate);
  addNewlyUnspentTransactionOutputs(blockTransactions,
                                    unspentTransactionOutputsToUpdate);
}

void Blockchain::removeNewlySpentTransactionOutputs(
    const vector<unique_ptr<const Transaction>> &newTransactions,
    unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputsToUpdate) {
  unordered_set<string> newlySpentTransactionOutputHashes;
  for_each(newTransactions.begin(), newTransactions.end(),
           [&](const unique_ptr<const Transaction> &transactionPtr) {
             unordered_set<string> transactionInputHashStrings =
                 TransactionPool::getTransactionInputHashStrings(
                     *transactionPtr);
             newlySpentTransactionOutputHashes.insert(
                 transactionInputHashStrings.begin(),
                 transactionInputHashStrings.end());
           });
  unordered_set<string> keysToDelete;
  for (const pair<string, UnspentTransactionOutput &> entry :
       unspentTransactionOutputsToUpdate) {
    if (newlySpentTransactionOutputHashes.find(entry.first) !=
        newlySpentTransactionOutputHashes.end()) {
      // this output has just been spent, need to remove it
      keysToDelete.insert(entry.first);
    }
  }
  for (const string &keyToDelete : keysToDelete) {
    unspentTransactionOutputsToUpdate.erase(keyToDelete);
  }
}

void Blockchain::addNewlyUnspentTransactionOutputs(
    const vector<unique_ptr<const Transaction>> &newTransactions,
    unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputsToUpdate) {
  for_each(newTransactions.begin(), newTransactions.end(),
           [&](const unique_ptr<const Transaction> &newTransactionPtr) {
             const vector<TransactionOutput> &newTransactionOutputs =
                 newTransactionPtr->getOutputs();
             for (int outputIndex = 0;
                  outputIndex < newTransactionOutputs.size(); outputIndex++) {
               UnspentTransactionOutput newUnspentTransactionOutput(
                   newTransactionPtr->getId(), outputIndex,
                   newTransactionOutputs.at(outputIndex).getAddress(),
                   newTransactionOutputs.at(outputIndex).getData());
               unspentTransactionOutputsToUpdate.insert(
                   {newUnspentTransactionOutput.getHashString(),
                    newUnspentTransactionOutput});
             }
           });
}
