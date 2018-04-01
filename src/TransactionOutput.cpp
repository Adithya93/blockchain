#include "TransactionOutput.h"
#include <sstream>
#include <vector>

TransactionOutput::TransactionOutput(const std::vector<unsigned char> &address,
                                     double data)
    : address(address), data(data) {}

const std::vector<unsigned char> &TransactionOutput::getAddress() const {
  return address;
}

const double TransactionOutput::getData() const { return data; }

std::string TransactionOutput::getHashString() const {
  return Util::stringFromUnsignedCharVecAndSerializableData(address, data);
}
