#ifndef SERIALIZABLE
#define SERIALIZABLE

#include <string>

class Serializable {
public:
  virtual std::string getHashString() const = 0;
};

#endif
