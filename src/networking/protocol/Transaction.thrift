
struct TransactionOutputMessage {
1: binary address,
2: double data,
}

struct TransactionInputMessage {
1: binary sourceTransactionId,
2: i32 sourceTransactionIndex,
3: binary signature,
}

struct UnspentTransactionOutputMessage {
1: binary sourceTransactionId,
2: i32 sourceTransactionIndex,
3: binary sourceTransactionAddress,
4: double data,
}

struct TransactionMessage {
1: binary transactionId,
2: list<TransactionInputMessage> transactionInputs,
3: list<TransactionOutputMessage> transactionOutputs,
}
