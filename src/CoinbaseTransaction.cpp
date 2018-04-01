#include "CoinbaseTransaction.h"

CoinbaseTransaction::CoinbaseTransaction(int blockIndex, EVP_PKEY &minerKeys,
                                         double miningReward)
    : Transaction(
          {generateCoinbaseTransactionInput(blockIndex)},
          {generateCoinbaseTransactionOutput(minerKeys, miningReward)}) {
  Transaction::signTransactionInputs(minerKeys);
}

CoinbaseTransaction::CoinbaseTransaction(
    const CoinbaseTransaction &signedTransactionToCopy)
    : Transaction(signedTransactionToCopy.getInputs(),
                  signedTransactionToCopy.getOutputs()) {}

CoinbaseTransaction::CoinbaseTransaction(
    const vector<TransactionInput> &signedTransactionInputs,
    const vector<TransactionOutput> &transactionOutputs)
    : Transaction(signedTransactionInputs, transactionOutputs) {}

bool CoinbaseTransaction::isValidTransaction(
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) const {
  return Transaction::computedHashMatchesRecordedHash() &&
         Transaction::transactionInputHasValidSignature(
             getInputs().front(), getOutputs().front().getAddress());
}

unique_ptr<Transaction> CoinbaseTransaction::copyTransaction() const {
  return unique_ptr<CoinbaseTransaction>(new CoinbaseTransaction(*this));
}

vector<TransactionInput>
CoinbaseTransaction::generateCoinbaseTransactionInput(int blockIndex) {
  return {TransactionInput(vector<unsigned char>(), blockIndex)};
}

vector<TransactionOutput>
CoinbaseTransaction::generateCoinbaseTransactionOutput(EVP_PKEY &minerKeys,
                                                       double data) {
  return {TransactionOutput(CryptoWallet::serializePublicKey(minerKeys), data)};
}
