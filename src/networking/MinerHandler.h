#ifndef MINER_HANDLER
#define MINER_HANDLER

#include "../Miner.h"
#include "protocol/gen-cpp/Miner.h"

class MinerHandler : public MinerIf {
public:
  MinerHandler(Miner &miner);
  bool
  addTransactionToPool(const TransactionMessage &transactionMessage) override;
  int32_t receiveTransactionPool(
      const std::vector<TransactionMessage> &transactionMessages) override;
  bool receiveNewBlock(const BlockMessage &blockMessage) override;
  bool receiveChain(const BlockchainMessage &blockchainMessage) override;
  void getLatestBlock(BlockMessage &_return) override;
  void getBlockchain(BlockchainMessage &_return) override;

private:
  Miner *miner;
};

#endif
