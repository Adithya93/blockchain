#include "GenesisBlock.h"
// TEMP
#include <iostream>

GenesisBlock::GenesisBlock()
    : Block(0,
            Block::computeHash(0, vector<unsigned char>(), GENESIS_TIMESTAMP,
                               vector<const Transaction *>(), 0, 0),
            vector<unsigned char>(), GENESIS_TIMESTAMP,
            vector<const Transaction *>(), 0, 0) {}

bool GenesisBlock::isValidNewBlock(
    const Block &previousBlock,
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) const {
  return Block::fulfillsConstructionInvariant();
}

// Genesis Block has no coinbase data, override check
bool GenesisBlock::hasCorrectCoinbaseData(double expectedCoinbaseData) const {
  return true;
}

unique_ptr<Block> GenesisBlock::copyBlock() const {
  return unique_ptr<GenesisBlock>(new GenesisBlock(*this));
}
