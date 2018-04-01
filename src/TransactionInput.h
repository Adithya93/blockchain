#ifndef TRANSACTION_INPUT
#define TRANSACTION_INPUT

#include "Serializable.h"
#include <vector>

class TransactionInput : public Serializable {
public:
  TransactionInput(const std::vector<unsigned char> sourceTransactionId,
                   int sourceTransactionIndex);
  const std::vector<unsigned char> &getSourceTransactionId() const;
  int getSourceTransactionIndex() const;
  const std::vector<unsigned char> &getSignature() const;
  void sign(const std::vector<unsigned char> signature);
  std::string getHashString() const override;

private:
  std::vector<unsigned char> sourceTransactionId;
  int sourceTransactionIndex;
  std::vector<unsigned char> signature;
};

#endif
