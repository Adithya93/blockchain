include "Blockchain.thrift"
include "Transaction.thrift"

//typedef Transaction.TransactionMessage TransactionMessage
//typedef Blockchain.BlockMessage BlockMessage
//typedef Blockchain.BlockchainMessage BlockchainMessage

service Miner {

bool addTransactionToPool(1: Transaction.TransactionMessage transaction),

i32 receiveTransactionPool(1: list<Transaction.TransactionMessage> transactions),

bool receiveNewBlock(1: Blockchain.BlockMessage newBlock),

bool receiveChain(1: Blockchain.BlockchainMessage chain),

Blockchain.BlockMessage getLatestBlock(),

Blockchain.BlockchainMessage getBlockchain(),

}
