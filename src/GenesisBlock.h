#ifndef GENESIS_BLOCK
#define GENESIS_BLOCK

#include "Block.h"

#define GENESIS_TIMESTAMP 1465154705

class GenesisBlock : public Block {
public:
  GenesisBlock();

  virtual bool
  isValidNewBlock(const Block &previousBlock,
                  const unordered_map<string, UnspentTransactionOutput>
                      &unspentTransactionOutputs) const override;

  bool hasCorrectCoinbaseData(double expectedCoinbaseData) const override;

  unique_ptr<Block> copyBlock() const override;
};

#endif
