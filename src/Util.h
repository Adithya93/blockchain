#ifndef UTIL
#define UTIL

#include "Serializable.h"
#include <string>
#include <vector>

using namespace std;

namespace Util {
string stringFromUnsignedCharVec(const vector<unsigned char> &vec);

string stringFromSerializableCollection(const Serializable *start,
                                        const Serializable *end);

string
stringFromUnsignedCharVecAndSerializableData(const vector<unsigned char> &vec,
                                             double data);

uint64_t generateTimeStamp();

struct StringHash {
public:
  size_t operator()(const Serializable &serializable) const;
};

} // namespace Util

#endif
