#include "Transaction.h"
#include "Util.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <sstream>

Transaction::Transaction(const vector<TransactionInput> &transactionInputs,
                         const vector<TransactionOutput> &transactionOutputs)
    : transactionInputs(transactionInputs),
      transactionOutputs(transactionOutputs) {
  assert(!transactionInputs.empty());
  assert(!transactionOutputs.empty());
  saveBulkStringData();
  computeAndSetHash();
}

Transaction::Transaction(const Transaction &transactionToCopy)
    : transactionInputs(transactionToCopy.getInputs()),
      transactionOutputs(transactionToCopy.getOutputs()) {
  Transaction tmp(transactionToCopy.getInputs(),
                  transactionToCopy.getOutputs());
  saveBulkStringData();
  computeAndSetHash();
}

// For overriding
unique_ptr<Transaction> Transaction::copyTransaction() const {
  return unique_ptr<Transaction>(new Transaction(*this));
}

bool Transaction::operator==(const Transaction &otherTransaction) const {
  return getHashString() == otherTransaction.getHashString();
}

void Transaction::computeAndSetHash() {
  id = Crypto::getSHA256Hash(bulkStringData);
}

void Transaction::signTransactionInputs(EVP_PKEY &privateKey) {
  const string &transactionData = bulkStringData;
  // Ensure atomicity - strong exception safety
  vector<TransactionInput> transactionInputsToSign(transactionInputs);
  exception_ptr eptr;
  for (TransactionInput &transactionInput : transactionInputsToSign) {
    transactionInput.sign(
        Crypto::signMessage(transactionData, privateKey, eptr));
  }
  if (eptr) {
    // TODO - Log instead of printing to stdout
    cout << "Error signing transaction input, aborting signing of transaction"
         << endl;
    rethrow_exception(eptr);
  }
  swap(transactionInputs, transactionInputsToSign);
}

bool Transaction::isValidTransaction(
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) const {
  return computedHashMatchesRecordedHash() &&
         allTransactionInputsValid(unspentTransactionOutputs);
}

const vector<unsigned char> &Transaction::getId() const { return id; }

const vector<TransactionInput> &Transaction::getInputs() const {
  return transactionInputs;
}

const vector<TransactionOutput> &Transaction::getOutputs() const {
  return transactionOutputs;
}

string Transaction::getHashString() const {
  return Util::stringFromUnsignedCharVec(id);
}

bool Transaction::computedHashMatchesRecordedHash() const {
  return getHashString(bulkStringData) == Util::stringFromUnsignedCharVec(id);
}

bool Transaction::transactionInputHasValidSignature(
    const TransactionInput &transactionInput,
    const vector<unsigned char> serializedPublicKey) const {
  exception_ptr eptr;
  bool result =
      Crypto::verifySignature(bulkStringData, transactionInput.getSignature(),
                              serializedPublicKey, eptr);
  if (eptr) {
    rethrow_exception(eptr);
  }
  return result;
}

bool Transaction::allTransactionInputsValid(
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) const {
  return all_of(transactionInputs.begin(), transactionInputs.end(),
                [&](const TransactionInput &transactionInput) {
                  return isValidTransactionInput(transactionInput,
                                                 unspentTransactionOutputs);
                });
}

bool Transaction::isValidTransactionInput(
    const TransactionInput &transactionInput,
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) const {
  auto foundIter = findUnspentTransactionOutputByIdAndIndex(
      unspentTransactionOutputs, transactionInput.getSourceTransactionId(),
      transactionInput.getSourceTransactionIndex());
  if (foundIter == unspentTransactionOutputs.end()) {
    // Referenced sourceTransactionId does not exist
    cout << "Referenced sourceTransaction not found: "
         << Util::stringFromUnsignedCharVec(
                transactionInput.getSourceTransactionId())
         << endl;
    return false;
  }
  auto serializedPublicKey = foundIter->second.getSourceTransactionAddress();
  // Verify signature
  if (!transactionInputHasValidSignature(transactionInput,
                                         serializedPublicKey)) {
    return false;
  }
  return true;
}

unordered_map<string, UnspentTransactionOutput>::const_iterator
Transaction::findUnspentTransactionOutputByIdAndIndex(
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs,
    const vector<unsigned char> &transactionId,
    int transactionOutputIndex) const {
  return unspentTransactionOutputs.find(
      Util::stringFromUnsignedCharVecAndSerializableData(
          transactionId, transactionOutputIndex));
}

string Transaction::getHashString(const string &data) const {
  return Util::stringFromUnsignedCharVec(Crypto::getSHA256Hash(data));
}

string Transaction::getBulkStringData() const {
  return (stringstream() << Util::stringFromSerializableCollection(
                                &(transactionInputs.front()),
                                &(transactionInputs.back()))
                         << Util::stringFromSerializableCollection(
                                &(transactionOutputs.front()),
                                &(transactionOutputs.back())))
      .str();
}

void Transaction::saveBulkStringData() { bulkStringData = getBulkStringData(); }
