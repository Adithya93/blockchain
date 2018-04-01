#ifndef TRANSACTION_OUTPUT
#define TRANSACTION_OUTPUT

#include "Serializable.h"
#include "Util.h"

class TransactionOutput : public Serializable {
public:
  TransactionOutput(const std::vector<unsigned char> &address, double data);

  const std::vector<unsigned char> &getAddress() const;

  const double getData() const;

  std::string getHashString() const override;

private:
  std::vector<unsigned char> address;
  double data;
};

#endif
