#include "Blockchain.h"
#include "CoinbaseTransaction.h"
#include "Crypto.h"
#include "CryptoWallet.h"
#include "Miner.h"
#include "Transaction.h"
#include "TransactionPool.h"
#include "Wallet.h"
#include "networking/MinerServer.h"
#include <iostream>

using namespace std;

#define PUBLIC_KEY_FILE "default_pub.pem"
#define PRIVATE_KEY_FILE "default_priv.pem"
// Just for testing
#define OTHER_PUBLIC_KEY_FILE "other_pub.pem"
#define OTHER_PRIVATE_KEY_FILE "other_priv.pem"

#define COINBASE_AMOUNT 100

Transaction generateTransactionFromMinerToRecipientFromBlock(
    const Block &block, EVP_PKEY &minerKeys,
    const vector<unsigned char> &recipientAddress) {
  const Transaction &coinbaseTransaction = *(block.getBlockData().front());
  const vector<unsigned char> &coinbaseTransactionId =
      coinbaseTransaction.getId();
  TransactionInput sourceTransaction(coinbaseTransactionId, 0);
  TransactionOutput grantFromMinerToRecipient(
      recipientAddress, coinbaseTransaction.getOutputs().front().getData());
  Transaction grantTransaction({sourceTransaction},
                               {grantFromMinerToRecipient});
  grantTransaction.signTransactionInputs(minerKeys);
  return grantTransaction;
}


/*
int main() {
  pair<EVP_PKEY &, EVP_PKEY &> minerKeyPair =
      CryptoWallet::readKeyPair(PUBLIC_KEY_FILE, PRIVATE_KEY_FILE);
  EVP_PKEY &minerPublicKey = minerKeyPair.first;
  EVP_PKEY &minerPrivateKey = minerKeyPair.second;
  EVP_PKEY &recipientKeys =
      CryptoWallet::generateKeys(OTHER_PUBLIC_KEY_FILE, OTHER_PRIVATE_KEY_FILE);
  vector<unsigned char> minerAddress(
      CryptoWallet::serializePublicKey(minerPublicKey));
  vector<unsigned char> recipientAddress(
      CryptoWallet::serializePublicKey(recipientKeys));

  cout << "Initializing server" << endl;
  MinerServer server(COINBASE_AMOUNT, minerPrivateKey);
  cout << "Launching server" << endl;
  // thread serverThread(&MinerServer::launchServer, server);
  server.launchServer();

  // cout << "Initializing wallet" << endl;
  // Wallet wallet;
  // serverThread.join();
}
*/
