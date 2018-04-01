/**
 * Autogenerated by Thrift Compiler (1.0.0-dev)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Miner_H
#define Miner_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include "Miner_types.h"



#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class MinerIf {
 public:
  virtual ~MinerIf() {}
  virtual bool addTransactionToPool(const  ::TransactionMessage& transaction) = 0;
  virtual int32_t receiveTransactionPool(const std::vector< ::TransactionMessage> & transactions) = 0;
  virtual bool receiveNewBlock(const  ::BlockMessage& newBlock) = 0;
  virtual bool receiveChain(const  ::BlockchainMessage& chain) = 0;
  virtual void getLatestBlock( ::BlockMessage& _return) = 0;
  virtual void getBlockchain( ::BlockchainMessage& _return) = 0;
};

class MinerIfFactory {
 public:
  typedef MinerIf Handler;

  virtual ~MinerIfFactory() {}

  virtual MinerIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(MinerIf* /* handler */) = 0;
};

class MinerIfSingletonFactory : virtual public MinerIfFactory {
 public:
  MinerIfSingletonFactory(const ::apache::thrift::stdcxx::shared_ptr<MinerIf>& iface) : iface_(iface) {}
  virtual ~MinerIfSingletonFactory() {}

  virtual MinerIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(MinerIf* /* handler */) {}

 protected:
  ::apache::thrift::stdcxx::shared_ptr<MinerIf> iface_;
};

class MinerNull : virtual public MinerIf {
 public:
  virtual ~MinerNull() {}
  bool addTransactionToPool(const  ::TransactionMessage& /* transaction */) {
    bool _return = false;
    return _return;
  }
  int32_t receiveTransactionPool(const std::vector< ::TransactionMessage> & /* transactions */) {
    int32_t _return = 0;
    return _return;
  }
  bool receiveNewBlock(const  ::BlockMessage& /* newBlock */) {
    bool _return = false;
    return _return;
  }
  bool receiveChain(const  ::BlockchainMessage& /* chain */) {
    bool _return = false;
    return _return;
  }
  void getLatestBlock( ::BlockMessage& /* _return */) {
    return;
  }
  void getBlockchain( ::BlockchainMessage& /* _return */) {
    return;
  }
};

typedef struct _Miner_addTransactionToPool_args__isset {
  _Miner_addTransactionToPool_args__isset() : transaction(false) {}
  bool transaction :1;
} _Miner_addTransactionToPool_args__isset;

class Miner_addTransactionToPool_args {
 public:

  Miner_addTransactionToPool_args(const Miner_addTransactionToPool_args&);
  Miner_addTransactionToPool_args& operator=(const Miner_addTransactionToPool_args&);
  Miner_addTransactionToPool_args() {
  }

  virtual ~Miner_addTransactionToPool_args() throw();
   ::TransactionMessage transaction;

  _Miner_addTransactionToPool_args__isset __isset;

  void __set_transaction(const  ::TransactionMessage& val);

  bool operator == (const Miner_addTransactionToPool_args & rhs) const
  {
    if (!(transaction == rhs.transaction))
      return false;
    return true;
  }
  bool operator != (const Miner_addTransactionToPool_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_addTransactionToPool_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Miner_addTransactionToPool_pargs {
 public:


  virtual ~Miner_addTransactionToPool_pargs() throw();
  const  ::TransactionMessage* transaction;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_addTransactionToPool_result__isset {
  _Miner_addTransactionToPool_result__isset() : success(false) {}
  bool success :1;
} _Miner_addTransactionToPool_result__isset;

class Miner_addTransactionToPool_result {
 public:

  Miner_addTransactionToPool_result(const Miner_addTransactionToPool_result&);
  Miner_addTransactionToPool_result& operator=(const Miner_addTransactionToPool_result&);
  Miner_addTransactionToPool_result() : success(0) {
  }

  virtual ~Miner_addTransactionToPool_result() throw();
  bool success;

  _Miner_addTransactionToPool_result__isset __isset;

  void __set_success(const bool val);

  bool operator == (const Miner_addTransactionToPool_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Miner_addTransactionToPool_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_addTransactionToPool_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_addTransactionToPool_presult__isset {
  _Miner_addTransactionToPool_presult__isset() : success(false) {}
  bool success :1;
} _Miner_addTransactionToPool_presult__isset;

class Miner_addTransactionToPool_presult {
 public:


  virtual ~Miner_addTransactionToPool_presult() throw();
  bool* success;

  _Miner_addTransactionToPool_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _Miner_receiveTransactionPool_args__isset {
  _Miner_receiveTransactionPool_args__isset() : transactions(false) {}
  bool transactions :1;
} _Miner_receiveTransactionPool_args__isset;

class Miner_receiveTransactionPool_args {
 public:

  Miner_receiveTransactionPool_args(const Miner_receiveTransactionPool_args&);
  Miner_receiveTransactionPool_args& operator=(const Miner_receiveTransactionPool_args&);
  Miner_receiveTransactionPool_args() {
  }

  virtual ~Miner_receiveTransactionPool_args() throw();
  std::vector< ::TransactionMessage>  transactions;

  _Miner_receiveTransactionPool_args__isset __isset;

  void __set_transactions(const std::vector< ::TransactionMessage> & val);

  bool operator == (const Miner_receiveTransactionPool_args & rhs) const
  {
    if (!(transactions == rhs.transactions))
      return false;
    return true;
  }
  bool operator != (const Miner_receiveTransactionPool_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_receiveTransactionPool_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Miner_receiveTransactionPool_pargs {
 public:


  virtual ~Miner_receiveTransactionPool_pargs() throw();
  const std::vector< ::TransactionMessage> * transactions;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_receiveTransactionPool_result__isset {
  _Miner_receiveTransactionPool_result__isset() : success(false) {}
  bool success :1;
} _Miner_receiveTransactionPool_result__isset;

class Miner_receiveTransactionPool_result {
 public:

  Miner_receiveTransactionPool_result(const Miner_receiveTransactionPool_result&);
  Miner_receiveTransactionPool_result& operator=(const Miner_receiveTransactionPool_result&);
  Miner_receiveTransactionPool_result() : success(0) {
  }

  virtual ~Miner_receiveTransactionPool_result() throw();
  int32_t success;

  _Miner_receiveTransactionPool_result__isset __isset;

  void __set_success(const int32_t val);

  bool operator == (const Miner_receiveTransactionPool_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Miner_receiveTransactionPool_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_receiveTransactionPool_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_receiveTransactionPool_presult__isset {
  _Miner_receiveTransactionPool_presult__isset() : success(false) {}
  bool success :1;
} _Miner_receiveTransactionPool_presult__isset;

class Miner_receiveTransactionPool_presult {
 public:


  virtual ~Miner_receiveTransactionPool_presult() throw();
  int32_t* success;

  _Miner_receiveTransactionPool_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _Miner_receiveNewBlock_args__isset {
  _Miner_receiveNewBlock_args__isset() : newBlock(false) {}
  bool newBlock :1;
} _Miner_receiveNewBlock_args__isset;

class Miner_receiveNewBlock_args {
 public:

  Miner_receiveNewBlock_args(const Miner_receiveNewBlock_args&);
  Miner_receiveNewBlock_args& operator=(const Miner_receiveNewBlock_args&);
  Miner_receiveNewBlock_args() {
  }

  virtual ~Miner_receiveNewBlock_args() throw();
   ::BlockMessage newBlock;

  _Miner_receiveNewBlock_args__isset __isset;

  void __set_newBlock(const  ::BlockMessage& val);

  bool operator == (const Miner_receiveNewBlock_args & rhs) const
  {
    if (!(newBlock == rhs.newBlock))
      return false;
    return true;
  }
  bool operator != (const Miner_receiveNewBlock_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_receiveNewBlock_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Miner_receiveNewBlock_pargs {
 public:


  virtual ~Miner_receiveNewBlock_pargs() throw();
  const  ::BlockMessage* newBlock;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_receiveNewBlock_result__isset {
  _Miner_receiveNewBlock_result__isset() : success(false) {}
  bool success :1;
} _Miner_receiveNewBlock_result__isset;

class Miner_receiveNewBlock_result {
 public:

  Miner_receiveNewBlock_result(const Miner_receiveNewBlock_result&);
  Miner_receiveNewBlock_result& operator=(const Miner_receiveNewBlock_result&);
  Miner_receiveNewBlock_result() : success(0) {
  }

  virtual ~Miner_receiveNewBlock_result() throw();
  bool success;

  _Miner_receiveNewBlock_result__isset __isset;

  void __set_success(const bool val);

  bool operator == (const Miner_receiveNewBlock_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Miner_receiveNewBlock_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_receiveNewBlock_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_receiveNewBlock_presult__isset {
  _Miner_receiveNewBlock_presult__isset() : success(false) {}
  bool success :1;
} _Miner_receiveNewBlock_presult__isset;

class Miner_receiveNewBlock_presult {
 public:


  virtual ~Miner_receiveNewBlock_presult() throw();
  bool* success;

  _Miner_receiveNewBlock_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _Miner_receiveChain_args__isset {
  _Miner_receiveChain_args__isset() : chain(false) {}
  bool chain :1;
} _Miner_receiveChain_args__isset;

class Miner_receiveChain_args {
 public:

  Miner_receiveChain_args(const Miner_receiveChain_args&);
  Miner_receiveChain_args& operator=(const Miner_receiveChain_args&);
  Miner_receiveChain_args() {
  }

  virtual ~Miner_receiveChain_args() throw();
   ::BlockchainMessage chain;

  _Miner_receiveChain_args__isset __isset;

  void __set_chain(const  ::BlockchainMessage& val);

  bool operator == (const Miner_receiveChain_args & rhs) const
  {
    if (!(chain == rhs.chain))
      return false;
    return true;
  }
  bool operator != (const Miner_receiveChain_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_receiveChain_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Miner_receiveChain_pargs {
 public:


  virtual ~Miner_receiveChain_pargs() throw();
  const  ::BlockchainMessage* chain;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_receiveChain_result__isset {
  _Miner_receiveChain_result__isset() : success(false) {}
  bool success :1;
} _Miner_receiveChain_result__isset;

class Miner_receiveChain_result {
 public:

  Miner_receiveChain_result(const Miner_receiveChain_result&);
  Miner_receiveChain_result& operator=(const Miner_receiveChain_result&);
  Miner_receiveChain_result() : success(0) {
  }

  virtual ~Miner_receiveChain_result() throw();
  bool success;

  _Miner_receiveChain_result__isset __isset;

  void __set_success(const bool val);

  bool operator == (const Miner_receiveChain_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Miner_receiveChain_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_receiveChain_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_receiveChain_presult__isset {
  _Miner_receiveChain_presult__isset() : success(false) {}
  bool success :1;
} _Miner_receiveChain_presult__isset;

class Miner_receiveChain_presult {
 public:


  virtual ~Miner_receiveChain_presult() throw();
  bool* success;

  _Miner_receiveChain_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class Miner_getLatestBlock_args {
 public:

  Miner_getLatestBlock_args(const Miner_getLatestBlock_args&);
  Miner_getLatestBlock_args& operator=(const Miner_getLatestBlock_args&);
  Miner_getLatestBlock_args() {
  }

  virtual ~Miner_getLatestBlock_args() throw();

  bool operator == (const Miner_getLatestBlock_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const Miner_getLatestBlock_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_getLatestBlock_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Miner_getLatestBlock_pargs {
 public:


  virtual ~Miner_getLatestBlock_pargs() throw();

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_getLatestBlock_result__isset {
  _Miner_getLatestBlock_result__isset() : success(false) {}
  bool success :1;
} _Miner_getLatestBlock_result__isset;

class Miner_getLatestBlock_result {
 public:

  Miner_getLatestBlock_result(const Miner_getLatestBlock_result&);
  Miner_getLatestBlock_result& operator=(const Miner_getLatestBlock_result&);
  Miner_getLatestBlock_result() {
  }

  virtual ~Miner_getLatestBlock_result() throw();
   ::BlockMessage success;

  _Miner_getLatestBlock_result__isset __isset;

  void __set_success(const  ::BlockMessage& val);

  bool operator == (const Miner_getLatestBlock_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Miner_getLatestBlock_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_getLatestBlock_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_getLatestBlock_presult__isset {
  _Miner_getLatestBlock_presult__isset() : success(false) {}
  bool success :1;
} _Miner_getLatestBlock_presult__isset;

class Miner_getLatestBlock_presult {
 public:


  virtual ~Miner_getLatestBlock_presult() throw();
   ::BlockMessage* success;

  _Miner_getLatestBlock_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class Miner_getBlockchain_args {
 public:

  Miner_getBlockchain_args(const Miner_getBlockchain_args&);
  Miner_getBlockchain_args& operator=(const Miner_getBlockchain_args&);
  Miner_getBlockchain_args() {
  }

  virtual ~Miner_getBlockchain_args() throw();

  bool operator == (const Miner_getBlockchain_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const Miner_getBlockchain_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_getBlockchain_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Miner_getBlockchain_pargs {
 public:


  virtual ~Miner_getBlockchain_pargs() throw();

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_getBlockchain_result__isset {
  _Miner_getBlockchain_result__isset() : success(false) {}
  bool success :1;
} _Miner_getBlockchain_result__isset;

class Miner_getBlockchain_result {
 public:

  Miner_getBlockchain_result(const Miner_getBlockchain_result&);
  Miner_getBlockchain_result& operator=(const Miner_getBlockchain_result&);
  Miner_getBlockchain_result() {
  }

  virtual ~Miner_getBlockchain_result() throw();
   ::BlockchainMessage success;

  _Miner_getBlockchain_result__isset __isset;

  void __set_success(const  ::BlockchainMessage& val);

  bool operator == (const Miner_getBlockchain_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Miner_getBlockchain_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Miner_getBlockchain_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Miner_getBlockchain_presult__isset {
  _Miner_getBlockchain_presult__isset() : success(false) {}
  bool success :1;
} _Miner_getBlockchain_presult__isset;

class Miner_getBlockchain_presult {
 public:


  virtual ~Miner_getBlockchain_presult() throw();
   ::BlockchainMessage* success;

  _Miner_getBlockchain_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class MinerClient : virtual public MinerIf {
 public:
  MinerClient(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  MinerClient(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  bool addTransactionToPool(const  ::TransactionMessage& transaction);
  void send_addTransactionToPool(const  ::TransactionMessage& transaction);
  bool recv_addTransactionToPool();
  int32_t receiveTransactionPool(const std::vector< ::TransactionMessage> & transactions);
  void send_receiveTransactionPool(const std::vector< ::TransactionMessage> & transactions);
  int32_t recv_receiveTransactionPool();
  bool receiveNewBlock(const  ::BlockMessage& newBlock);
  void send_receiveNewBlock(const  ::BlockMessage& newBlock);
  bool recv_receiveNewBlock();
  bool receiveChain(const  ::BlockchainMessage& chain);
  void send_receiveChain(const  ::BlockchainMessage& chain);
  bool recv_receiveChain();
  void getLatestBlock( ::BlockMessage& _return);
  void send_getLatestBlock();
  void recv_getLatestBlock( ::BlockMessage& _return);
  void getBlockchain( ::BlockchainMessage& _return);
  void send_getBlockchain();
  void recv_getBlockchain( ::BlockchainMessage& _return);
 protected:
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class MinerProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  ::apache::thrift::stdcxx::shared_ptr<MinerIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (MinerProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_addTransactionToPool(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_receiveTransactionPool(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_receiveNewBlock(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_receiveChain(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_getLatestBlock(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_getBlockchain(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  MinerProcessor(::apache::thrift::stdcxx::shared_ptr<MinerIf> iface) :
    iface_(iface) {
    processMap_["addTransactionToPool"] = &MinerProcessor::process_addTransactionToPool;
    processMap_["receiveTransactionPool"] = &MinerProcessor::process_receiveTransactionPool;
    processMap_["receiveNewBlock"] = &MinerProcessor::process_receiveNewBlock;
    processMap_["receiveChain"] = &MinerProcessor::process_receiveChain;
    processMap_["getLatestBlock"] = &MinerProcessor::process_getLatestBlock;
    processMap_["getBlockchain"] = &MinerProcessor::process_getBlockchain;
  }

  virtual ~MinerProcessor() {}
};

class MinerProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  MinerProcessorFactory(const ::apache::thrift::stdcxx::shared_ptr< MinerIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::apache::thrift::stdcxx::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::apache::thrift::stdcxx::shared_ptr< MinerIfFactory > handlerFactory_;
};

class MinerMultiface : virtual public MinerIf {
 public:
  MinerMultiface(std::vector<apache::thrift::stdcxx::shared_ptr<MinerIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~MinerMultiface() {}
 protected:
  std::vector<apache::thrift::stdcxx::shared_ptr<MinerIf> > ifaces_;
  MinerMultiface() {}
  void add(::apache::thrift::stdcxx::shared_ptr<MinerIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  bool addTransactionToPool(const  ::TransactionMessage& transaction) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->addTransactionToPool(transaction);
    }
    return ifaces_[i]->addTransactionToPool(transaction);
  }

  int32_t receiveTransactionPool(const std::vector< ::TransactionMessage> & transactions) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->receiveTransactionPool(transactions);
    }
    return ifaces_[i]->receiveTransactionPool(transactions);
  }

  bool receiveNewBlock(const  ::BlockMessage& newBlock) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->receiveNewBlock(newBlock);
    }
    return ifaces_[i]->receiveNewBlock(newBlock);
  }

  bool receiveChain(const  ::BlockchainMessage& chain) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->receiveChain(chain);
    }
    return ifaces_[i]->receiveChain(chain);
  }

  void getLatestBlock( ::BlockMessage& _return) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->getLatestBlock(_return);
    }
    ifaces_[i]->getLatestBlock(_return);
    return;
  }

  void getBlockchain( ::BlockchainMessage& _return) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->getBlockchain(_return);
    }
    ifaces_[i]->getBlockchain(_return);
    return;
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class MinerConcurrentClient : virtual public MinerIf {
 public:
  MinerConcurrentClient(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  MinerConcurrentClient(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  bool addTransactionToPool(const  ::TransactionMessage& transaction);
  int32_t send_addTransactionToPool(const  ::TransactionMessage& transaction);
  bool recv_addTransactionToPool(const int32_t seqid);
  int32_t receiveTransactionPool(const std::vector< ::TransactionMessage> & transactions);
  int32_t send_receiveTransactionPool(const std::vector< ::TransactionMessage> & transactions);
  int32_t recv_receiveTransactionPool(const int32_t seqid);
  bool receiveNewBlock(const  ::BlockMessage& newBlock);
  int32_t send_receiveNewBlock(const  ::BlockMessage& newBlock);
  bool recv_receiveNewBlock(const int32_t seqid);
  bool receiveChain(const  ::BlockchainMessage& chain);
  int32_t send_receiveChain(const  ::BlockchainMessage& chain);
  bool recv_receiveChain(const int32_t seqid);
  void getLatestBlock( ::BlockMessage& _return);
  int32_t send_getLatestBlock();
  void recv_getLatestBlock( ::BlockMessage& _return, const int32_t seqid);
  void getBlockchain( ::BlockchainMessage& _return);
  int32_t send_getBlockchain();
  void recv_getBlockchain( ::BlockchainMessage& _return, const int32_t seqid);
 protected:
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
  ::apache::thrift::async::TConcurrentClientSyncInfo sync_;
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif



#endif
