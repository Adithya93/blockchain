#ifndef TRANSACTION
#define TRANSACTION

#include "Crypto.h"
#include "Serializable.h"
#include "TransactionInput.h"
#include "TransactionOutput.h"
#include "UnspentTransactionOutput.h"
#include <unordered_map>
#include <vector>

using namespace std;

class Transaction : public Serializable {
public:
  Transaction(const vector<TransactionInput> &transactionInputs,
              const vector<TransactionOutput> &transactionOutputs);

  Transaction(const Transaction &transactionToCopy);

  // For overriding
  virtual unique_ptr<Transaction> copyTransaction() const;

  bool operator==(const Transaction &otherTransaction) const;

  void computeAndSetHash();

  void signTransactionInputs(EVP_PKEY &privateKey);

  virtual bool
  isValidTransaction(const unordered_map<string, UnspentTransactionOutput>
                         &unspentTransactionOutputs) const;

  const vector<unsigned char> &getId() const;

  const vector<TransactionInput> &getInputs() const;

  const vector<TransactionOutput> &getOutputs() const;

  string getHashString() const override;

protected:
  bool computedHashMatchesRecordedHash() const;

  bool transactionInputHasValidSignature(
      const TransactionInput &transactionInput,
      const vector<unsigned char> serializedPublicKey) const;

private:
  vector<unsigned char> id;
  vector<TransactionInput> transactionInputs;
  vector<TransactionOutput> transactionOutputs;
  string bulkStringData;

  bool allTransactionInputsValid(
      const unordered_map<string, UnspentTransactionOutput>
          &unspentTransactionOutputs) const;

  bool
  isValidTransactionInput(const TransactionInput &transactionInput,
                          const unordered_map<string, UnspentTransactionOutput>
                              &unspentTransactionOutputs) const;

  unordered_map<string, UnspentTransactionOutput>::const_iterator
  findUnspentTransactionOutputByIdAndIndex(
      const unordered_map<string, UnspentTransactionOutput>
          &unspentTransactionOutputs,
      const vector<unsigned char> &transactionId,
      int transactionOutputIndex) const;

  string getHashString(const string &data) const;

  string getBulkStringData() const;

  void saveBulkStringData();
};

#endif
