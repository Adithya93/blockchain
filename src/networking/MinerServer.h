#include "../Crypto.h"
#include "../Miner.h"

class MinerServer {
public:
  MinerServer(double miningReward, EVP_PKEY &minerKeys);

  void launchServer();

private:
  Miner miner;
};
