#include "Util.h"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace Util {
string stringFromUnsignedCharVec(const vector<unsigned char> &vec) {
  return string(vec.begin(), vec.end());
}

string stringFromSerializableCollection(const Serializable *start,
                                        const Serializable *end) {
  stringstream ss;
  for (const Serializable *serializable = start; serializable <= end;
       serializable++) {
    ss << serializable->getHashString();
  }
  return ss.str();
}

string
stringFromUnsignedCharVecAndSerializableData(const vector<unsigned char> &vec,
                                             double data) {
  return (stringstream() << stringFromUnsignedCharVec(vec) << data).str();
}

uint64_t generateTimeStamp() {
  return chrono::duration_cast<chrono::seconds>(
             chrono::system_clock::now().time_since_epoch())
      .count();
}

size_t StringHash::operator()(const Serializable &serializable) const {
  return hash<string>()(serializable.getHashString());
}

} // namespace Util
