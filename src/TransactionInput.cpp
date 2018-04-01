#include "TransactionInput.h"
#include "Util.h"
#include <sstream>

TransactionInput::TransactionInput(
    const std::vector<unsigned char> sourceTransactionId,
    int sourceTransactionIndex)
    : sourceTransactionId(sourceTransactionId),
      sourceTransactionIndex(sourceTransactionIndex) {}

const std::vector<unsigned char> &
TransactionInput::getSourceTransactionId() const {
  return sourceTransactionId;
}

int TransactionInput::getSourceTransactionIndex() const {
  return sourceTransactionIndex;
}

const std::vector<unsigned char> &TransactionInput::getSignature() const {
  return signature;
}

void TransactionInput::sign(const std::vector<unsigned char> signature) {
  this->signature = signature;
}

std::string TransactionInput::getHashString() const {
  return Util::stringFromUnsignedCharVecAndSerializableData(
      this->getSourceTransactionId(), this->getSourceTransactionIndex());
}
