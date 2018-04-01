#include "Crypto.h"
#include "CryptoCommons.h"
#include <iostream>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <sstream>

#define OPENSSL_ERROR_STRING_MAX_LEN 240
// TODO - Need to thread-safe method of error-handling, or remove exception
// throws in favor of error flags
#define ERROR_MESSAGE_DELIMITER " : "
#define INVALID_PUBLIC_KEY_SERIALIZATION "Invalid public key serialization"
#define DIGEST_INITIALIZATION_ERROR                                            \
  "Crypto error while initializing hash digest context"
#define MESSAGE_SIGN_DIGEST_INITIALIZATION_ERROR                               \
  "Crypto error while initializing signature generation context"
#define DIGEST_UPDATE_ERROR "Crypto error while hashing"
#define OPENSSL_MALLOC_ERROR "Crypto error while allocating memory from OpenSSL"
#define MESSAGE_SIGN_DIGEST_LENGTH_COMPUTATION_ERROR                           \
  "Crypto error while computing hash digest length"
#define MESSAGE_SIGN_DIGEST_FINALIZATION_ERROR "Crypto error while signing hash"
#define SIGNATURE_VERIFICATION_CRYPTO_ERROR                                    \
  "Crypto error while verifying signature"

void CryptoCommons::initializeOpenSSL() {
  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  OpenSSL_add_all_ciphers();
}

const string getOpenSSLErrorMessage(unsigned long errorCode) {
  char errorMessageBuffer[OPENSSL_ERROR_STRING_MAX_LEN + 1];
  ERR_error_string_n(errorCode, errorMessageBuffer,
                     OPENSSL_ERROR_STRING_MAX_LEN);
  errorMessageBuffer[OPENSSL_ERROR_STRING_MAX_LEN] = '\0';
  string errorMessageString(errorMessageBuffer);
  return errorMessageString;
}

const string generateContextualizedErrorMessage(const string &contextString,
                                                unsigned long errorCode) {
  return (stringstream() << contextString << ERROR_MESSAGE_DELIMITER
                         << getOpenSSLErrorMessage(errorCode))
      .str();
}

EVP_PKEY &deserializePublicKey(const vector<unsigned char> &serializedPublicKey,
                               int *res) {
  EVP_PKEY *pkeyHolder = NULL;
  const unsigned char *keyBytes = &serializedPublicKey.front();
  int keyLength = serializedPublicKey.size();
  d2i_PUBKEY(&pkeyHolder, &keyBytes, keyLength);
  *res = pkeyHolder != NULL;
  return *pkeyHolder;
}

void cleanUpCryptoData(int success, unsigned char *sigBuffer,
                       EVP_MD_CTX *mdctx) {
  if (success != 1) {
    /* Do some error handling */
    cout << "gg teddy" << endl;
  }
  /* Clean up */
  if (sigBuffer && !success)
    OPENSSL_free(sigBuffer);
  if (mdctx)
    EVP_MD_CTX_destroy(mdctx);
}

// Private Variant
vector<unsigned char> signMessage(const string msgstr, EVP_PKEY &pkeyRef,
                                  int *success) {
  vector<unsigned char> signature;
  *success = 0;
  EVP_PKEY *pkey = &pkeyRef;
  EVP_MD_CTX *mdctx = NULL;
  unsigned char *sig = NULL;
  const char *msg = msgstr.c_str();
  size_t slen;
  vector<unsigned char> message_digest;
  try {
    /* Create the Message Digest Context */
    if (!(mdctx = EVP_MD_CTX_create())) {
      throw runtime_error(generateContextualizedErrorMessage(
          DIGEST_INITIALIZATION_ERROR, ERR_get_error()));
    }
    /* Initialise the DigestSign operation - SHA-256 has been selected as the
     * message digest function in this example */
    if (1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, pkey)) {
      throw runtime_error(generateContextualizedErrorMessage(
          MESSAGE_SIGN_DIGEST_INITIALIZATION_ERROR, ERR_get_error()));
    }
    /* Call update with the message */
    if (1 != EVP_DigestSignUpdate(mdctx, msg, strlen(msg))) {
      throw runtime_error(generateContextualizedErrorMessage(
          DIGEST_UPDATE_ERROR, ERR_get_error()));
    }
    /* Finalise the DigestSign operation */
    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the
     * length of the signature. Length is returned in slen */
    if (1 != EVP_DigestSignFinal(mdctx, NULL, &slen)) {
      throw runtime_error(generateContextualizedErrorMessage(
          MESSAGE_SIGN_DIGEST_LENGTH_COMPUTATION_ERROR, ERR_get_error()));
    }
    /* Allocate memory for the signature based on size in slen */
    if (!(sig = (unsigned char *)OPENSSL_malloc(sizeof(unsigned char) *
                                                (slen)))) {
      throw runtime_error(generateContextualizedErrorMessage(
          OPENSSL_MALLOC_ERROR, ERR_get_error()));
    }
    /* Obtain the signature */
    if (1 != EVP_DigestSignFinal(mdctx, sig, &slen)) {
      throw runtime_error(generateContextualizedErrorMessage(
          MESSAGE_SIGN_DIGEST_FINALIZATION_ERROR, ERR_get_error()));
    }
    /* Success */
    *success = 1;
    signature = vector<unsigned char>(sig, sig + slen);
  } catch (runtime_error &e) {
    cleanUpCryptoData(*success, sig, mdctx);
    throw(e);
  }
  cleanUpCryptoData(*success, sig, mdctx);
  return signature;
}

// Private Variant
bool verifySignature(const string msgstr, const vector<unsigned char> signature,
                     EVP_PKEY &pkeyRef) {
  EVP_MD_CTX *mdctx = NULL;
  const char *msg = msgstr.c_str();
  const unsigned char *sig = &signature.front();
  int sigLen = signature.size();
  EVP_PKEY *key = &pkeyRef;
  if (!(mdctx = EVP_MD_CTX_create())) {
    // Handle error
    throw runtime_error(generateContextualizedErrorMessage(
        SIGNATURE_VERIFICATION_CRYPTO_ERROR, ERR_get_error()));
  }
  try {
    /* Initialize `key` with a public key */
    if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, key)) {
      cout << "Unable to initialize digest for signature verification" << endl;
      throw runtime_error(generateContextualizedErrorMessage(
          SIGNATURE_VERIFICATION_CRYPTO_ERROR, ERR_get_error()));
    }
    if (1 != EVP_DigestVerifyUpdate(mdctx, msg, strlen(msg))) {
      throw runtime_error(generateContextualizedErrorMessage(
          SIGNATURE_VERIFICATION_CRYPTO_ERROR, ERR_get_error()));
    }
    if (EVP_DigestVerifyFinal(mdctx, sig, sigLen) == 1) {
      EVP_MD_CTX_destroy(mdctx);
      return true;
    } else {
      cout << "Error verifying signature" << endl;
      throw runtime_error(generateContextualizedErrorMessage(
          SIGNATURE_VERIFICATION_CRYPTO_ERROR, ERR_get_error()));
    }
  } catch (runtime_error &e) {
    EVP_MD_CTX_destroy(mdctx);
    throw e;
    return false;
  }
}

namespace Crypto {

void initializeOpenSSL() { return CryptoCommons::initializeOpenSSL(); }

const vector<unsigned char> getSHA256Hash(const string &input) {
  EVP_MD_CTX *digestContext = EVP_MD_CTX_new();
  // TODO - Handle case of digest context initialization failure
  EVP_DigestInit(digestContext, EVP_sha256());
  EVP_DigestUpdate(digestContext, input.c_str(), input.length());
  unsigned char *hashBuffer = (unsigned char *)OPENSSL_malloc(
      EVP_MD_size(EVP_sha256()) * sizeof(unsigned char));
  unsigned int hashLen;
  EVP_DigestFinal(digestContext, hashBuffer, &hashLen);
  vector<unsigned char> hashByteVector(hashBuffer, hashBuffer + hashLen);
  OPENSSL_free(hashBuffer);
  EVP_MD_CTX_destroy(digestContext);
  return hashByteVector;
}

// Public Variant
vector<unsigned char> signMessage(const string msgstr, EVP_PKEY &pkeyRef,
                                  exception_ptr &eptr) {
  try {
    int success;
    return signMessage(msgstr, pkeyRef, &success);
  } catch (const exception &e) {
    eptr = current_exception();
    return vector<unsigned char>();
  }
}

// Public Variant
bool verifySignature(const string msgstr, const vector<unsigned char> signature,
                     const vector<unsigned char> &serializedPublicKey,
                     exception_ptr &eptr) {
  try {
    int success;
    EVP_PKEY &deserializedPublicKey =
        deserializePublicKey(serializedPublicKey, &success);
    if (!success) {
      throw invalid_argument(INVALID_PUBLIC_KEY_SERIALIZATION);
    }
    return verifySignature(msgstr, signature, deserializedPublicKey);
  } catch (const exception &e) {
    eptr = current_exception();
    return false;
  }
}

} // namespace Crypto
