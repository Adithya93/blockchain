#include "SerializationUtils.h"
#include "../../CoinbaseTransaction.h"
#include <iostream>

using namespace std;

inline vector<unsigned char>
unsignedCharVecFromByteString(const string &byteString) {
  return vector<unsigned char>(byteString.begin(), byteString.end());
}

TransactionInput deserializeTransactionInputMessage(
    const TransactionInputMessage &transactionInputMessage) {
  TransactionInput transactionInput(
      unsignedCharVecFromByteString(
          transactionInputMessage.sourceTransactionId),
      transactionInputMessage.sourceTransactionIndex);
  transactionInput.sign(
      unsignedCharVecFromByteString(transactionInputMessage.signature));
  return transactionInput;
}

inline TransactionOutput deserializeTransactionOutputMessage(
    const TransactionOutputMessage &transactionOutputMessage) {
  return TransactionOutput(
      unsignedCharVecFromByteString(transactionOutputMessage.address),
      transactionOutputMessage.data);
}

template <typename T, typename R>
vector<R> deserializeMessages(const vector<T> &messages,
                              R (*messageDeserializer)(const T &)) {
  vector<R> deserializedMessages;
  transform(messages.begin(), messages.end(),
            back_inserter(deserializedMessages), messageDeserializer);
  return deserializedMessages;
}

vector<TransactionInput> deserializeTransactionInputMessages(
    const vector<TransactionInputMessage> &transactionInputMessages) {
  return deserializeMessages<TransactionInputMessage, TransactionInput>(
      transactionInputMessages, deserializeTransactionInputMessage);
}

vector<TransactionOutput> deserializeTransactionOutputMessages(
    const vector<TransactionOutputMessage> &transactionOutputMessages) {
  return deserializeMessages<TransactionOutputMessage, TransactionOutput>(
      transactionOutputMessages, deserializeTransactionOutputMessage);
}

template <typename T, typename R>
vector<R> deserializeMessagesInOrder(const vector<T> &messages,
                                     R (*messageDeserializer)(const T &)) {
  vector<R> deserializedMessages;
  for (const T &message : messages) {
    deserializedMessages.push_back(messageDeserializer(message));
  }
  return deserializedMessages;
}

inline vector<Transaction> deserializeTransactionMessagesInOrder(
    const vector<TransactionMessage> &transactionMessages) {
  return deserializeMessagesInOrder<TransactionMessage, Transaction>(
      transactionMessages, SerializationUtils::deserializeTransactionMessage);
}

inline vector<Block>
deserializeBlockMessagesInOrder(const vector<BlockMessage> &blockMessages) {
  return deserializeMessagesInOrder<BlockMessage, Block>(
      blockMessages, SerializationUtils::deserializeBlockMessage);
}

TransactionInputMessage
serializeTransactionInput(const TransactionInput &transactionInput) {
  TransactionInputMessage transactionInputMessage;
  transactionInputMessage.sourceTransactionId = Util::stringFromUnsignedCharVec(
      transactionInput.getSourceTransactionId());
  transactionInputMessage.sourceTransactionIndex =
      transactionInput.getSourceTransactionIndex();
  transactionInputMessage.signature =
      Util::stringFromUnsignedCharVec(transactionInput.getSignature());
  return transactionInputMessage;
}

TransactionOutputMessage
serializeTransactionOutput(const TransactionOutput &transactionOutput) {
  TransactionOutputMessage transactionOutputMessage;
  transactionOutputMessage.address =
      Util::stringFromUnsignedCharVec(transactionOutput.getAddress());
  transactionOutputMessage.data = transactionOutput.getData();
  return transactionOutputMessage;
}

template <typename T, typename R>
vector<R> serializeDataCollection(const vector<T> &dataCollection,
                                  R (*dataSerializer)(const T &)) {
  vector<R> serializedData;
  transform(dataCollection.begin(), dataCollection.end(),
            back_inserter(serializedData), dataSerializer);
  return serializedData;
}

inline vector<TransactionInputMessage>
serializeTransactionInputs(const vector<TransactionInput> &transactionInputs) {
  return serializeDataCollection<TransactionInput, TransactionInputMessage>(
      transactionInputs, serializeTransactionInput);
}

inline vector<TransactionOutputMessage> serializeTransactionOutputs(
    const vector<TransactionOutput> &transactionOutputs) {
  return serializeDataCollection<TransactionOutput, TransactionOutputMessage>(
      transactionOutputs, serializeTransactionOutput);
}

template <typename T, typename R>
vector<R>
serializeDataCollectionInOrder(const vector<unique_ptr<const T>> &dataPtrs,
                               R (*dataSerializer)(const T &)) {
  vector<R> serializedData;
  for (const unique_ptr<const T> &dataPtr : dataPtrs) {
    serializedData.push_back(dataSerializer(*dataPtr));
  }
  return serializedData;
}

inline vector<TransactionMessage> serializeBlockTransactionsInOrder(
    const vector<unique_ptr<const Transaction>> &transactionPtrs) {
  return serializeDataCollectionInOrder(
      transactionPtrs, SerializationUtils::serializeTransaction);
}

inline vector<BlockMessage>
serializeChainBlocksInOrder(const vector<unique_ptr<const Block>> &blockPtrs) {
  return serializeDataCollectionInOrder(blockPtrs,
                                        SerializationUtils::serializeBlock);
}

namespace SerializationUtils {
TransactionMessage serializeTransaction(const Transaction &transaction) {
  TransactionMessage transactionMessage;
  transactionMessage.transactionId =
      Util::stringFromUnsignedCharVec(transaction.getId());
  transactionMessage.transactionInputs =
      serializeTransactionInputs(transaction.getInputs());
  transactionMessage.transactionOutputs =
      serializeTransactionOutputs(transaction.getOutputs());
  return transactionMessage;
}

BlockMessage serializeBlock(const Block &block) {
  BlockMessage blockMessage;
  blockMessage.blockHash =
      Util::stringFromUnsignedCharVec(block.getBlockHash());
  blockMessage.blockIndex = block.getBlockIndex();
  blockMessage.previousBlockHash =
      Util::stringFromUnsignedCharVec(block.getPreviousBlockHash());
  blockMessage.data = serializeBlockTransactionsInOrder(block.getBlockData());
  blockMessage.difficulty = block.getDifficulty();
  blockMessage.nonce = block.getNonce();
  blockMessage.timestamp = block.getBlockTimestamp();
  return blockMessage;
}

BlockchainMessage serializeBlockchain(const Blockchain &blockchain) {
  BlockchainMessage blockchainMessage;
  blockchainMessage.difficulty = blockchain.getCurrentDifficulty();
  blockchainMessage.miningReward = blockchain.getMiningReward();
  blockchainMessage.chain =
      serializeChainBlocksInOrder(blockchain.getChainCopy());
  return blockchainMessage;
}

Transaction
deserializeTransactionMessage(const TransactionMessage &transactionMessage) {
  return Transaction(
      deserializeTransactionInputMessages(transactionMessage.transactionInputs),
      deserializeTransactionOutputMessages(
          transactionMessage.transactionOutputs));
}

Block deserializeBlockMessage(const BlockMessage &blockMessage) {
  const vector<Transaction> blockTransactions =
      deserializeTransactionMessagesInOrder(blockMessage.data);
  vector<const Transaction *> transactionPtrs;
  // To keep blockCoinbaseTransactionPtr in scope
  unique_ptr<CoinbaseTransaction> blockCoinbaseTransactionPtr;
  if (blockTransactions.size() > 0) {
    for (const Transaction &blockTransaction : blockTransactions) {
      transactionPtrs.push_back(&blockTransaction);
    }
    // Restore CoinbaseTransaction

    blockCoinbaseTransactionPtr = unique_ptr<CoinbaseTransaction>(
        new CoinbaseTransaction(transactionPtrs.front()->getInputs(),
                                transactionPtrs.front()->getOutputs()));
    transactionPtrs.at(0) = blockCoinbaseTransactionPtr.get();
  }
  return Block(blockMessage.blockIndex,
               unsignedCharVecFromByteString(blockMessage.blockHash),
               unsignedCharVecFromByteString(blockMessage.previousBlockHash),
               blockMessage.timestamp, transactionPtrs, blockMessage.difficulty,
               blockMessage.nonce);
}

// TODO - Improve performance? A lot of expensive copying ...
Blockchain
deserializeBlockchainMessage(const BlockchainMessage &blockchainMessage) {
  cout << "Deserializing blockchain message" << endl;
  const vector<Block> chainBlocks =
      deserializeBlockMessagesInOrder(blockchainMessage.chain);
  vector<const Block *> blockPtrs;
  // TODO - Handle edge / error case of empty blockchain?
  for (const Block &block : chainBlocks) {
    blockPtrs.push_back(&block);
  }
  // Restore Genesis Block
  const GenesisBlock genesis;
  blockPtrs.at(0) = &genesis;
  return Blockchain(blockchainMessage.miningReward,
                    blockchainMessage.difficulty, blockPtrs);
}

} // namespace SerializationUtils
