#ifndef CRYPTO
#define CRYPTO

#include <openssl/evp.h>
#include <vector>

using namespace std;

namespace Crypto {

void initializeOpenSSL();

const vector<unsigned char> getSHA256Hash(const string &input);

// Perhaps move to CryptoWallet instead?
vector<unsigned char> signMessage(string msgstr, EVP_PKEY &pkeyRef,
                                  exception_ptr &e);

bool verifySignature(const string msgstr, const vector<unsigned char> signature,
                     const vector<unsigned char> &serializedPublicKey,
                     exception_ptr &e);

} // namespace Crypto

#endif
