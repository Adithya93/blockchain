#include "MinerHandler.h"
#include "protocol/SerializationUtils.h"
#include <algorithm>

using namespace std;

MinerHandler::MinerHandler(Miner &miner) : miner(&miner) {}

bool MinerHandler::addTransactionToPool(
    const TransactionMessage &transactionMessage) {
  return miner->addTransactionToPool(
      SerializationUtils::deserializeTransactionMessage(transactionMessage));
}

int32_t MinerHandler::receiveTransactionPool(
    const vector<TransactionMessage> &transactionMessages) {
  vector<Transaction> decodedTransactions;
  // Order of pool transactions shouldn't matter, use transform for faster
  // out-of-order processing
  transform(transactionMessages.begin(), transactionMessages.end(),
            back_inserter(decodedTransactions),
            [&](const TransactionMessage &transactionMessage) {
              return SerializationUtils::deserializeTransactionMessage(
                  transactionMessage);
            });
  return miner->receiveTransactionPool(decodedTransactions);
}

bool MinerHandler::receiveNewBlock(const BlockMessage &blockMessage) {
  return miner->receiveNewBlock(
      SerializationUtils::deserializeBlockMessage(blockMessage));
}

bool MinerHandler::receiveChain(const BlockchainMessage &blockchainMessage) {
  return miner->receiveChain(
      SerializationUtils::deserializeBlockchainMessage(blockchainMessage));
}

void MinerHandler::getLatestBlock(BlockMessage &_return) {
  _return = SerializationUtils::serializeBlock(miner->getLatestBlock());
}

void MinerHandler::getBlockchain(BlockchainMessage &_return) {
  _return = SerializationUtils::serializeBlockchain(miner->getBlockchain());
}
