#include "UnspentTransactionOutput.h"
#include "Util.h"
#include <functional>
#include <sstream>

UnspentTransactionOutput::UnspentTransactionOutput(
    const vector<unsigned char> &sourceTransactionId,
    int sourceTransactionIndex,
    const vector<unsigned char> &sourceTransactionAddress, double data)
    : sourceTransactionId(sourceTransactionId),
      sourceTransactionIndex(sourceTransactionIndex),
      sourceTransactionAddress(sourceTransactionAddress), data(data) {}

const vector<unsigned char> &
UnspentTransactionOutput::getSourceTransactionId() const {
  return sourceTransactionId;
}

const int UnspentTransactionOutput::getSourceTransactionIndex() const {
  return sourceTransactionIndex;
}

const vector<unsigned char> &
UnspentTransactionOutput::getSourceTransactionAddress() const {
  return sourceTransactionAddress;
}

const double UnspentTransactionOutput::getData() const { return data; }

string UnspentTransactionOutput::getHashString() const {
  return Util::stringFromUnsignedCharVecAndSerializableData(
      sourceTransactionId, sourceTransactionIndex);
}
