
#include "TransactionPool.h"
#include <algorithm>
#include <iostream>

unordered_set<Transaction, Util::StringHash>
TransactionPool::getTransactionPool() const {
  lock_guard<mutex> poolMutexLck(transactionPoolMutex);
  return transactions;
}

bool TransactionPool::addToTransactionPool(
    const Transaction &newTransaction,
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) {
  lock_guard<mutex> poolMutexLck(transactionPoolMutex);
  return addToTransactionPoolUnsynchronized(newTransaction,
                                            unspentTransactionOutputs);
}

void TransactionPool::updateTransactionPool(
    const unordered_map<string, UnspentTransactionOutput>
        &updatedUnspentTransactionOutputs) {
  unordered_set<Transaction, Util::StringHash> validTransactions;
  lock_guard<mutex> poolMutexLck(transactionPoolMutex);
  copy_if(transactions.begin(), transactions.end(),
          inserter(validTransactions, validTransactions.begin()),
          [&](const Transaction &poolTransaction) {
            return all_of(poolTransaction.getInputs().begin(),
                          poolTransaction.getInputs().end(),
                          [&](const TransactionInput &transactionInput) {
                            return updatedUnspentTransactionOutputs.find(
                                       transactionInput.getHashString()) !=
                                   updatedUnspentTransactionOutputs.end();
                          });
          });
  swap(transactions, validTransactions);
  // TODO - consider logging removed transactions?
}

int TransactionPool::mergeWithTransactionPool(
    const vector<Transaction> &transactions,
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) {
  int numMerged = 0;
  lock_guard<mutex> poolMutexLck(transactionPoolMutex);
  for_each(transactions.begin(), transactions.end(),
           [&](const Transaction &transaction) {
             if (addToTransactionPoolUnsynchronized(
                     transaction, unspentTransactionOutputs)) {
               numMerged++;
             }
           });
  return numMerged;
}

unordered_set<string> TransactionPool::getTransactionInputHashStrings(
    const Transaction &transaction) {
  unordered_set<string> transactionInputHashStrings;
  transform(transaction.getInputs().begin(), transaction.getInputs().end(),
            inserter(transactionInputHashStrings,
                     transactionInputHashStrings.begin()),
            [](const TransactionInput &transactionInput) {
              return transactionInput.getHashString();
            });
  return transactionInputHashStrings;
}

bool TransactionPool::isValidTransactionForPool(
    const Transaction &newTransaction) const {
  unordered_set<string> poolInputHashStrings =
      getPoolTransactionInputHashStrings();
  for (const string &inputHashString :
       getTransactionInputHashStrings(newTransaction)) {
    if (poolInputHashStrings.find(inputHashString) !=
        poolInputHashStrings.end()) {
      cout << "Received transaction tries to spend output already spent by "
              "transaction in pool!"
           << endl;
      return false;
    }
  }
  return true;
}

unordered_set<string>
TransactionPool::getPoolTransactionInputHashStrings() const {
  unordered_set<string> poolInputHashStrings;
  for_each(transactions.begin(), transactions.end(),
           [&](const Transaction &transaction) {
             unordered_set<string> inputHashStringsFromTransaction =
                 getTransactionInputHashStrings(transaction);
             poolInputHashStrings.insert(
                 inputHashStringsFromTransaction.begin(),
                 inputHashStringsFromTransaction.end());
           });
  return poolInputHashStrings;
}

bool TransactionPool::addToTransactionPoolUnsynchronized(
    const Transaction &newTransaction,
    const unordered_map<string, UnspentTransactionOutput>
        &unspentTransactionOutputs) {
  if (newTransaction.isValidTransaction(unspentTransactionOutputs) &&
      isValidTransactionForPool(newTransaction)) {
    return transactions.insert(newTransaction).second;
  }
  // TODO - Logging of invalid transaction or double-spend attack
  cout << "Invalid transaction received in pool!" << endl;
  return false;
}
