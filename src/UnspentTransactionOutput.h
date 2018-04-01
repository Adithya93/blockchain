#ifndef UNSPENT_TRANSACTION_OUTPUT
#define UNSPENT_TRANSACTION_OUTPUT

#include "Serializable.h"
#include "Util.h"
#include <vector>

using namespace std;

class UnspentTransactionOutput : public Serializable {
public:
  UnspentTransactionOutput(
      const vector<unsigned char> &sourceTransactionId,
      int sourceTransactionIndex,
      const vector<unsigned char> &sourceTransactionAddress, double data);

  const vector<unsigned char> &getSourceTransactionId() const;

  const int getSourceTransactionIndex() const;

  const vector<unsigned char> &getSourceTransactionAddress() const;

  const double getData() const;

  string getHashString() const override;

private:
  vector<unsigned char> sourceTransactionId;
  int sourceTransactionIndex;
  // Should the following 2 just be computed on the fly and not stored to avoid
  // duplication ? Not a consistency concern since immutable ...
  vector<unsigned char> sourceTransactionAddress;
  double data;
};

#endif
