include "Transaction.thrift"

//typedef Transaction.TransactionMessage TransactionMessage

struct BlockMessage {
1: i32 blockIndex,
2: binary blockHash,
3: binary previousBlockHash,
4: i64 timestamp,
5: list<Transaction.TransactionMessage> data,
6: i32 difficulty,
7: i64 nonce,
}

struct BlockchainMessage {
1: double miningReward,
2: i32 difficulty,
3: list<BlockMessage> chain,
}
