#ifndef COINBASE_TRANSACTION
#define COINBASE_TRANSACTION

#include "CryptoWallet.h"
#include "Transaction.h"

class CoinbaseTransaction : public Transaction {
public:
  // Use this one for signing
  CoinbaseTransaction(int blockIndex, EVP_PKEY &minerKeys, double miningReward);

  // Will assume signedTransactionToCopy is already signed
  CoinbaseTransaction(const CoinbaseTransaction &signedTransactionToCopy);

  // Will assume transaction inputs are already signed
  CoinbaseTransaction(const vector<TransactionInput> &signedTransactionInputs,
                      const vector<TransactionOutput> &transactionOutputs);

  bool isValidTransaction(const unordered_map<string, UnspentTransactionOutput>
                              &unspentTransactionOutputs) const override;

  unique_ptr<Transaction> copyTransaction() const override;

private:
  vector<TransactionInput> generateCoinbaseTransactionInput(int blockIndex);

  vector<TransactionOutput>
  generateCoinbaseTransactionOutput(EVP_PKEY &minerKeys, double data);
};

#endif
