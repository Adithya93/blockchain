#include "CryptoCommons.h"
#include "CryptoWallet.h"
#include <iostream>
#include <openssl/pem.h>
#include <vector>

using namespace std;

#define ECCTYPE "secp521r1"
#define KEY_SERIALIZATION_ERROR "Crypto error while serializing key"

namespace CryptoWallet {

void initializeOpenSSL() { CryptoCommons::initializeOpenSSL(); }

pair<EVP_PKEY &, EVP_PKEY &> readKeyPair(const char *publicKeyFileName,
                                         const char *privateKeyFileName) {
  BIO *privInBio = BIO_new(BIO_s_file());
  BIO *pubInBio = BIO_new(BIO_s_file());
  BIO_read_filename(privInBio, privateKeyFileName);
  BIO_read_filename(pubInBio, publicKeyFileName);
  // Extract private and public keys
  EC_KEY *pubKey, *privKey;
  pubKey = PEM_read_bio_EC_PUBKEY(pubInBio, NULL, NULL, NULL);
  privKey = PEM_read_bio_ECPrivateKey(privInBio, NULL, NULL, NULL);
  EVP_PKEY *publicKeyWrapper = EVP_PKEY_new();
  EVP_PKEY *privateKeyWrapper = EVP_PKEY_new();

  // Private key should contain public key info as well
  EC_KEY_set_public_key(privKey, EC_KEY_get0_public_key(pubKey));
  //

  EVP_PKEY_assign_EC_KEY(publicKeyWrapper, pubKey);
  EVP_PKEY_assign_EC_KEY(privateKeyWrapper, privKey);
  if (!publicKeyWrapper) {
    // TODO - More graceful error handling?
    cout << "Error reading public key!" << endl;
    exit(1);
  }
  if (!privateKeyWrapper) {
    // TODO - More graceful error handling?
    cout << "Error reading private key!" << endl;
    exit(1);
  }
  BIO_free_all(privInBio);
  BIO_free_all(pubInBio);
  return pair<EVP_PKEY &, EVP_PKEY &>(*publicKeyWrapper, *privateKeyWrapper);
}

EVP_PKEY &generateKeys(const char *publicKeyFileName,
                       const char *privateKeyFileName) {
  /* ------------------------------------------------------------ *
   * purpose:     Example code for creating elliptic curve        *
   *              cryptography (ECC) key pairs                    *
   * author:      01/26/2015 Frank4DD                             *
   *                                                              *
   * ------------------------------------------------------------ */
  BIO *outBio, *pubOutbio, *privOutBio;
  EC_KEY *myecc = NULL;
  EVP_PKEY *pkey = NULL;
  int eccgrp;
  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  outBio = BIO_new(BIO_s_file());
  pubOutbio = BIO_new(BIO_s_file());
  privOutBio = BIO_new(BIO_s_file());
  outBio = BIO_new_fp(stdout, BIO_NOCLOSE);
  pubOutbio = BIO_new_fp(fopen(publicKeyFileName, "w"), BIO_CLOSE);
  privOutBio = BIO_new_fp(fopen(privateKeyFileName, "w"), BIO_CLOSE);
  /* ---------------------------------------------------------- *
   * Create a EC key sructure, setting the group type from NID  *
   * ---------------------------------------------------------- */
  eccgrp = OBJ_txt2nid(ECCTYPE);
  myecc = EC_KEY_new_by_curve_name(eccgrp);
  /* -------------------------------------------------------- *
   * For cert signing, we use  the OPENSSL_EC_NAMED_CURVE flag*
   * ---------------------------------------------------------*/
  EC_KEY_set_asn1_flag(myecc, OPENSSL_EC_NAMED_CURVE);
  /* -------------------------------------------------------- *
   * Create the public/private EC key pair here               *
   * ---------------------------------------------------------*/
  if (!(EC_KEY_generate_key(myecc)))
    BIO_printf(outBio, "Error generating the ECC key.");
  /* -------------------------------------------------------- *
   * Converting the EC key into a PKEY structure let us       *
   * handle the key just like any other key pair.             *
   * ---------------------------------------------------------*/
  pkey = EVP_PKEY_new();
  if (!EVP_PKEY_assign_EC_KEY(pkey, myecc))
    BIO_printf(outBio, "Error assigning ECC key to EVP_PKEY structure.");
  /* -------------------------------------------------------- *
   * Now we show how to extract EC-specifics from the key     *
   * ---------------------------------------------------------*/
  myecc = EVP_PKEY_get0_EC_KEY(pkey);
  const EC_GROUP *ecgrp = EC_KEY_get0_group(myecc);
  /* ---------------------------------------------------------- *
   * Here we print the key length, and extract the curve type.  *
   * ---------------------------------------------------------- */
  BIO_printf(outBio, "ECC Key size: %d bit\n", EVP_PKEY_bits(pkey));
  BIO_printf(outBio, "ECC Key type: %s\n",
             OBJ_nid2sn(EC_GROUP_get_curve_name(ecgrp)));
  /* ---------------------------------------------------------- *
   * Here we print the private/public key data in PEM format.   *
   * ---------------------------------------------------------- */
  if (!PEM_write_bio_PrivateKey(privOutBio, pkey, NULL, NULL, 0, 0, NULL))
    BIO_printf(privOutBio, "Error writing private key data in PEM format");

  if (!PEM_write_bio_PUBKEY(pubOutbio, pkey))
    BIO_printf(pubOutbio, "Error writing public key data in PEM format");
  BIO_free_all(privOutBio);
  BIO_free_all(pubOutbio);
  return *pkey;
}

vector<unsigned char> serializePublicKey(EVP_PKEY &pkey) {
  unsigned char *keyBytes = NULL;
  int keyLen = i2d_PUBKEY(&pkey, NULL);
  if (!(i2d_PUBKEY(&pkey, &keyBytes))) {
    throw invalid_argument(KEY_SERIALIZATION_ERROR);
  }
  vector<unsigned char> keyBytesVector(keyBytes, keyBytes + keyLen);
  free(keyBytes);
  return keyBytesVector;
}

vector<unsigned char> serializePrivateKey(EVP_PKEY &pkey) {
  unsigned char *keyBytes = NULL;
  int keyLen = i2d_PrivateKey(&pkey, NULL);
  if (!(i2d_PrivateKey(&pkey, &keyBytes))) {
    throw invalid_argument(KEY_SERIALIZATION_ERROR);
  }
  vector<unsigned char> keyBytesVector(keyBytes, keyBytes + keyLen);
  free(keyBytes);
  return keyBytesVector;
}

} // namespace CryptoWallet
