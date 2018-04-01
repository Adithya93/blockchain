#ifndef CRYPTO_WALLET
#define CRYPTO_WALLET

#include <openssl/evp.h>
#include <utility>
#include <vector>

namespace CryptoWallet {

void initializeOpenSSL();

std::pair<EVP_PKEY &, EVP_PKEY &> readKeyPair(const char *publicKeyFileName,
                                              const char *privateKeyFileName);

EVP_PKEY &generateKeys(const char *publicKeyFileName,
                       const char *privateKeyFileName);

std::vector<unsigned char> serializePublicKey(EVP_PKEY &pkey);

std::vector<unsigned char> serializePrivateKey(EVP_PKEY &pkey);

} // namespace CryptoWallet

#endif
