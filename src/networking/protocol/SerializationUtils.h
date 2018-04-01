#ifndef SERIALIZATION_UTILS
#define SERIALIZATION_UTILS

#include "../../Blockchain.h"
#include "gen-cpp/Miner.h"

namespace SerializationUtils {
TransactionMessage serializeTransaction(const Transaction &transaction);

BlockMessage serializeBlock(const Block &block);

BlockchainMessage serializeBlockchain(const Blockchain &blockchain);

Transaction
deserializeTransactionMessage(const TransactionMessage &transactionMessage);

Block deserializeBlockMessage(const BlockMessage &blockMessage);

Blockchain
deserializeBlockchainMessage(const BlockchainMessage &blockchainMessage);

} // namespace SerializationUtils

#endif
