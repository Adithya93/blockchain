#ifndef TRANSACTION_POOL
#define TRANSACTION_POOL

#include "Transaction.h"
#include "Util.h"
#include <thread>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class TransactionPool {
public:
  // Copy for safer concurrency
  unordered_set<Transaction, Util::StringHash> getTransactionPool() const;

  bool
  addToTransactionPool(const Transaction &newTransaction,
                       const unordered_map<string, UnspentTransactionOutput>
                           &unspentTransactionOutputs);

  void
  updateTransactionPool(const unordered_map<string, UnspentTransactionOutput>
                            &updatedUnspentTransactionOutputs);

  int mergeWithTransactionPool(
      const vector<Transaction> &transactions,
      const unordered_map<string, UnspentTransactionOutput>
          &unspentTransactionOutputs);

  static unordered_set<string>
  getTransactionInputHashStrings(const Transaction &transaction);

private:
  unordered_set<Transaction, Util::StringHash> transactions;
  // Synchronize potentially concurrent updates of transaction pool
  mutable mutex transactionPoolMutex;

  bool isValidTransactionForPool(const Transaction &newTransaction) const;

  unordered_set<string> getPoolTransactionInputHashStrings() const;

  // Helper for addToTransactionPool
  bool addToTransactionPoolUnsynchronized(
      const Transaction &newTransaction,
      const unordered_map<string, UnspentTransactionOutput>
          &unspentTransactionOutputs);
};

#endif
