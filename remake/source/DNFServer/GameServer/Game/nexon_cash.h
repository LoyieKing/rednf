#ifndef DNF_NEXON_CASH_H
#define DNF_NEXON_CASH_H

// ============================================================================
// nexon::cash 域权威声明（NEXON 计费连接层：socket / 缓冲 / 事务）
// 反汇编基线：df_game_r ORIG _ZN5nexon4cash* 符号
// 类属主批次：OnlyOrig_Nexon（Wave5）。
// 所有签名取自 ORIG mangled；成员偏移标注自反汇编。
// ============================================================================

#include <map>
#include <vector>

struct timeval;   // <sys/time.h> 由 TU 引入

namespace nexon
{
namespace cash
{

// ---------------------------------------------------------------------------
// TCPSocket —— BSD TCP 封装（独立类，非继承层级）
// 布局（ctor 0x081af4cc）：
//   +0x00 int    m_socket (-1)
//   +0x04 char   m_pad04[0x10]      （本类未引用，保留区）
//   +0x14 u32    m_peerAdrs         （网络序 in_addr）
//   +0x18 u16    m_peerPort         （网络序存储，getPeerPort 原样返回）
//   +0x1c u32    m_sendRetryCount
// sizeof == 0x20（NCashImpl ctor 内嵌于 +0x20..+0x40 区间验证）
// ---------------------------------------------------------------------------
class TCPSocket
{
public:
    TCPSocket();                                        // ORIG 0x081af4cc
    explicit TCPSocket(const char* host, unsigned short port); // ORIG 0x081af50e*
    ~TCPSocket();                                       // ORIG 0x081af51e → close()

    bool open();                                        // ORIG 0x081af532
    bool bind(unsigned short port, bool nonBlock);      // ORIG 0x081af5b4
    bool listen(int backlog);                           // ORIG 0x081af674
    int  send(char* buffer, unsigned int length);       // ORIG 0x081af6ae
    int  recv(char* buffer, unsigned int length);       // ORIG 0x081af76e
    void close();                                       // ORIG 0x081af80c
    int  getHandle() const;                             // ORIG 0x081af7f4
    // ORIG 0x081af7fe (sz 0xd)：仅比较 m_socket==-1 且丢弃结果、立即返回；
    // 无任何系统调用。忠实保留为无副作用函数。
    void shutdown(int how);

    bool connect(const char* host, unsigned short port);// ORIG 0x081af9ea
    bool connect_nonb(const char* host, unsigned short port,
                      timeval timeout);                 // ORIG 0x081afa98
    bool accept(TCPSocket& peer);                       // ORIG 0x081aff2c

    bool setOptNonBlock();                              // ORIG 0x081af85e
    bool setOptReuseAdrs(bool on);                      // ORIG 0x081af8bc
    bool setOptLinger(bool on);                         // ORIG 0x081af926
    bool setOptNagle(bool on);                          // ORIG 0x081af990
    bool setOptResizeSendBuf(int size);                 // ORIG 0x081affd0
    bool setOptResizeRecvBuf(int size);                 // ORIG 0x081b0036
    static void SetRecvBufSize(int size);               // ORIG 0x081b009c
    static void SetSendBufSize(int size);               // ORIG 0x081b00aa

    unsigned int*  getPeerAdrs();                       // ORIG 0x081affb8 → &this->m_peerAdrs
    unsigned short getPeerPort();                       // ORIG 0x081affc4
    bool pollReadEvent() const;                         // ORIG 0x081afcc8
    bool pollWriteEvent() const;                        // ORIG 0x081afd94
    bool pollErrorEvent() const;                        // ORIG 0x081afe60

    static const unsigned int MAX_SEND_RETRY = 0x64;    // R 数据 0x08bb49e0 (=100)

private:
    TCPSocket(const TCPSocket&);            // ORIG 无复制符号
    TCPSocket& operator=(const TCPSocket&);

private:
    int            m_socket;         // +0x00
    char           m_pad04[0x10];    // +0x04（未引用保留区）
    unsigned int   m_peerAdrs;       // +0x14
    unsigned short m_peerPort;       // +0x18
    unsigned int   m_sendRetryCount; // +0x1c

public:
    static int msRecvBufSize;        // B 0x09407d68
    static int msSendBufSize;        // B 0x09407d6c
};

// ---------------------------------------------------------------------------
// UDPSocket
// 布局（ctor 0x081aed2c、bind/recvfrom 使用点）：
//   +0x00 int             m_socket(-1)
//   +0x04 sockaddr_in     m_adrs（bind 目标，sin_port 存网络序）
//   +0x14 unsigned short  m_port（主机序镜像）
//   +0x18 sockaddr_in     m_from（recvfrom 回填来源）
// ---------------------------------------------------------------------------
class UDPSocket
{
public:
    UDPSocket();                                        // ORIG 0x081aed2c
    ~UDPSocket();                                       // ORIG 0x081aed44 → close()

    bool open();                                        // ORIG 0x081aed58
    bool bind(const char* host, unsigned short port);   // ORIG 0x081aedc6
    bool bind(unsigned short port, bool nonBlock);      // ORIG 0x081aee82
    int  send(char* data, int length, unsigned short port,
              const char* host);                        // ORIG 0x081af000
    int  recv(char* buffer, int length);                // ORIG 0x081af0de
    int  getHandle();                                   // ORIG 0x081af17a
    void close();                                       // ORIG 0x081af184
    bool setOptNonBlock();                              // ORIG 0x081aefa2
    bool setOptResizeSendBuf(int size);                 // ORIG 0x081af1b8
    bool setOptResizeRecvBuf(int size);                 // ORIG 0x081af210
    bool pollReadEvent() const;                         // ORIG 0x081af268
    bool pollWriteEvent() const;                        // ORIG 0x081af334
    bool pollErrorEvent() const;                        // ORIG 0x081af400

private:
    UDPSocket(const UDPSocket&);
    UDPSocket& operator=(const UDPSocket&);

private:
    int            m_socket;  // +0x00
    unsigned char  m_adrs[16];// +0x04 sockaddr_in（避免系统头污染全局作用域）
    unsigned short m_port;    // +0x14
    unsigned char  m_from[16];// +0x18
};

// ---------------------------------------------------------------------------
// ByteBuffer —— 大端序读写游标缓冲
// 布局（ctor 0x081ae9e2）：+0x00 char* read 起点；+0x04 char* write 游标；
// +0x08 char* read 游标；+0x0c unsigned int capacity。
// length() = write 游标 - read 起点（0x081acf60）。
// ---------------------------------------------------------------------------
class ByteBuffer
{
public:
    ByteBuffer(char* buffer, unsigned int capacity);    // ORIG 0x081ae9e2
    ByteBuffer& wrap(char* buffer, unsigned int capacity); // ORIG 0x081ae9b8（按值返回 this）

    unsigned int length() const;                        // ORIG 0x081acf60

    ByteBuffer& operator<<(const char* value);          // ORIG 0x081acba0
    ByteBuffer& operator<<(const wchar_t* value);       // ORIG 0x081acd1e
    ByteBuffer& operator<<(unsigned char value);        // ORIG 0x081ae614
    ByteBuffer& operator<<(int value);                  // ORIG 0x081ae6c4（大端 4B）
    ByteBuffer& operator<<(unsigned int value);         // ORIG 0x081ae74c（大端 4B）
    ByteBuffer& operator<<(short value);                // ORIG 0x081ae65e（大端 2B）
    ByteBuffer& operator<<(unsigned short value);       // ORIG 0x081acefa（大端 2B）
    ByteBuffer& operator<<(unsigned long long value);   // ORIG 0x081ae7d4（大端 8B）

    ByteBuffer& operator>>(unsigned char& value);       // ORIG 0x081ae8bc
    ByteBuffer& operator>>(int& value);                 // ORIG 0x081ae8e4（大端读出）
    ByteBuffer& operator>>(unsigned int& value);        // ORIG 0x081ae94e

private:
    char*         m_start;      // +0x00
    char*         m_writePtr;   // +0x04
    char*         m_readPtr;    // +0x08
    unsigned int  m_capacity;   // +0x0c
};

// ---------------------------------------------------------------------------
// NCashMessage —— 计费消息帧
// 布局（ctor 0x081ae52e / 各访问器）：
//   +0x00 unsigned char m_magic('U'=0x55)
//   +0x02 unsigned short m_bodyLength（主机序镜像；decode 时由网络序还原）
//   +0x04 raw message 区起点（data()）；encode_header 写 +4=0x55、+5=u16 htons(bodyLen)
//   +0x07 body() 起始
// decode_header 校验 bodyLen<=0x200，超过则清零并返 false（0x081ae5ce）。
// ---------------------------------------------------------------------------
class NCashMessage
{
public:
    NCashMessage();                                     // ORIG 0x081ae52e

    unsigned char* data();                              // ORIG 0x081ae542 → this+4
    unsigned char* body();                              // ORIG 0x081ae560 → this+7
    unsigned short body_length();                       // ORIG 0x081ae56c
    int length() const;                                 // ORIG 0x081ae54e = body_length()+3
    void encode_header(unsigned int bodyLength);        // ORIG 0x081ae5ae
    bool encode_header();                               // ORIG 0x081ae578
    bool decode_header();                               // ORIG 0x081ae5ce

private:
    NCashMessage(const NCashMessage&);
    NCashMessage& operator=(const NCashMessage&);

private:
    unsigned char  m_magic;       // +0x00
    unsigned char  m_pad01;       // +0x01（ctor 不初始化）
    unsigned short m_bodyLength;  // +0x02 主机序
    unsigned char  m_raw[3];      // +0x04 报文头原始区（data() 视图起点）
    // 注意：实际对象尺寸由 NCashImpl 内嵌区间推得为 0x208（+0x40→+0x248 下一个子对象），
    // 余下空间即接收缓冲本体（自 +0x07 起）。
};

// ---------------------------------------------------------------------------
// NXTransaction —— 计费事务 POD。ORIG 无任何成员符号（无 C1/D1，非虚）。
// 字段实证自 NexonCash::confirm/cancel 的转发拆参（h,j）：
//   +0x00 unsigned char 类型；+0x04 unsigned int 事务 ID。其余未访问。
// ---------------------------------------------------------------------------
struct NXTransaction
{
    unsigned char m_type;      // +0x00
    unsigned char m_pad01[3];  // +0x01 对齐保留
    unsigned int  m_id;        // +0x04
};

// ---------------------------------------------------------------------------
// Transaction —— 单笔计费事务（ctor 0x081b02ec 布局）
//   +0x00 unsigned int m_id；+0x04 int m_amount；
//   +0x08 int m_createdTime；+0x0c UserBalance* m_userBalance
// ---------------------------------------------------------------------------
class UserBalance;

// ---------------------------------------------------------------------------
// Transaction —— 单笔计费事务（ctor 0x081b02ec 布局）
//   +0x00 unsigned int m_id；+0x04 int m_amount；
//   +0x08 int m_createdTime；+0x0c UserBalance* m_userBalance
// ---------------------------------------------------------------------------
class Transaction
{
public:
    Transaction(UserBalance* userBalance, int amount); // ORIG 0x081b02ec

    unsigned int id() const;                           // ORIG 0x081b050c
    bool expired() const;                              // ORIG 0x081b1656
    UserBalance* getUser() const;                      // ORIG 0x081b0516

    int  execute();                                    // ORIG 0x081b0336
    void confirm();                                    // ORIG 0x081b0398
    void cancel();                                     // ORIG 0x081b03b4

private:
    unsigned int m_id;            // +0x00
    int          m_amount;        // +0x04
    int          m_createdTime;   // +0x08
    UserBalance* m_userBalance;   // +0x0c
};

// ---------------------------------------------------------------------------
// UserBalance —— +0x04 int 余额；+0x08 vector<Transaction*>。
// ORIG 无本类 ctor/dtor 符号，禁止发明。（+0x00 本域函数未访问）
// ---------------------------------------------------------------------------
class UserBalance
{
public:
    int  balance() const;                              // ORIG 0x081b0522 W
    void balance(int delta);                           // ORIG 0x081b052e W（+=）
    void addNewTransaction(Transaction* tx);           // ORIG 0x081b15ca
    void removeTransaction(Transaction* tx);           // ORIG 0x081b1530
    void expire();                                     // ORIG 0x081b147c

private:
    char                          m_pad00[4];     // +0x00 未访问区
    int                           m_balance;      // +0x04
    std::vector<Transaction*>     m_transactions; // +0x08
};

// ---------------------------------------------------------------------------
// TransactionManager —— id→Transaction* 注册表单例。
// +0x00 std::map<unsigned int, Transaction*> m_transactions。
// 静态：int tx_id_ @ORIG 0x09407d90；instance 为 guard 单例(0x09407db8/dc0)。
// ---------------------------------------------------------------------------
class TransactionManager
{
public:
    static TransactionManager& instance();             // ORIG 0x081b0128
    static unsigned int getNextTransactionID();        // ORIG 0x081b04cc W

    TransactionManager();                              // ORIG 0x081b04f8 W
    ~TransactionManager();                             // ORIG 0x081b0544 W

    void addTransaction(Transaction* tx);              // ORIG 0x081b01bc
    void removeTransaction(Transaction* tx);           // ORIG 0x081b021c
    Transaction* getTransaction(unsigned int id);      // ORIG 0x081b0444 W
    void cancel(unsigned int id);                      // ORIG 0x081b0244
    void confirm(unsigned int id);                     // ORIG 0x081b0298

private:
    TransactionManager(const TransactionManager&);
    TransactionManager& operator=(const TransactionManager&);

private:
    std::map<unsigned int, Transaction*> m_transactions; // +0x00

    static int tx_id_;                                 // B 0x09407d90
};
}  // namespace cash
}  // namespace nexon

#endif  // DNF_NEXON_CASH_H
