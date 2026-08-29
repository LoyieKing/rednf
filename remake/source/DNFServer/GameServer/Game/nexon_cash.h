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

// ORIG 死常量（头文件 static，逐 TU 一份；值已按 .rodata 实测）：
static const int INVALID_SOCKET = -1;                 // _ZN5nexon4cashL14INVALID_SOCKETE r 0x4 (0xffffffff)
static const unsigned int MAX_NEXON_ID_LENTH = 0x20;  // _ZN5nexon4cashL18MAX_NEXON_ID_LENTHE r 0x4 (=32)
static const unsigned int _MAX_PATH = 0x104;          // _ZN5nexon4cashL9_MAX_PATHE r 0x4 (=260)
static const unsigned short C_ARAD_PROD_TYPE = 0x2d;  // _ZN5nexon4cashL16C_ARAD_PROD_TYPEE r 0x2 (=45)

// ---------------------------------------------------------------------------
// TCPSocket —— BSD TCP 封装（独立类，非继承层级）

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
    unsigned char  m_raw[0x204];  // +0x04 报文原始区（data() 视图起点）+ 接收缓冲本体
    // sizeof = 0x208（+0x40→+0x248 下一个子对象，NCashImpl 内嵌两实例总 0x450）
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
// ---------------------------------------------------------------------------
// NCashImpl —— 计费服务端连接实现（内嵌 TCPSocket + 收发消息帧）。
// 布局（ctor 0x081acfdc）：
//   +0x00 unsigned char m_paramB      （第3参 unsigned char）
//   +0x02 unsigned short m_fieldC2    （第4参 unsigned short）
//   +0x04 short          m_fieldD     （第5参 short）
//   +0x08 unsigned int   m_requestID  （ctor 置 0，_increaseRequestID ++）
//   +0x0c bool           m_connected  （ctor false）
//   +0x0d char           m_serverAdrs[0x11] （strcpy 目标；至多到 +0x1e 前区）
//   +0x1e unsigned short m_port       （第二参 short 端口原值）
//   +0x20 TCPSocket
//   +0x40 NCashMessage   m_inMsg      （区间 0x208：接收缓冲本体自 data()/body() 起）
//   +0x248 NCashMessage  m_outMsg
// sizeof == 0x450（NexonCash ctor new(0x450) 实证）。
// ---------------------------------------------------------------------------
class NCashImpl
{
public:
    NCashImpl(const char* serverAdrs, short port, unsigned char paramB,
              unsigned short fieldC, short fieldD);          // ORIG 0x081acfdc

    void _increaseRequestID();                               // ORIG 0x081aea32
    bool checkCoupon();                                      // ORIG 0x081aea0a（恒 false）
    bool useCoupon();                                        // ORIG 0x081aea14（恒 false）
    bool checkCafeCoupon();                                  // ORIG 0x081aea1e（恒 false）
    bool useCafeCoupon();                                    // ORIG 0x081aea28（恒 false）

    int recv_msg();                                          // ORIG 0x081aea46
    int recv_body();                                         // ORIG 0x081aeb3c
    int send_msg(unsigned int bodyLength);                   // ORIG 0x081aec2c
    bool reconnect();                                        // ORIG 0x081ad080

    ByteBuffer set_data_header(unsigned char packetId, unsigned char flag); // ORIG 0x081ad0e6 (sret 形态，按值返回)
    void get_data_header(unsigned char& a, unsigned char& b,
                         unsigned int& c);                   // ORIG 0x081ad168
    int handle_cash_error(int code);                         // ORIG 0x081ad1e0
    int handle_coupon_error(int code);                       // ORIG 0x081ad2c0

    int checkEntrance(const char* adrs, char& out);          // ORIG 0x081ad316
    int getBalance(const char* adrs, int& balance);          // ORIG 0x081ad49e
    int purchase(unsigned long long sn, int productNo, int count,
                 const char* idStr, const char* extraId,
                 NXTransaction& tx);                         // ORIG 0x081ad5c2
    int purchase(unsigned long long sn, int productNo, int count,
                 const char* idStr, const char* extraId,
                 NXTransaction& tx, short category);         // ORIG 0x081ad7ae
    int gift(unsigned long long sn, int productNo, int count,
             const char* idStr, const char* extraId,
             const char* recvName, const char* message,
             NXTransaction& tx);                             // ORIG 0x081ad9c4
    int confirm(unsigned char type, unsigned int txId);      // ORIG 0x081adbd4
    int cancel(unsigned char type, unsigned int txId);       // ORIG 0x081add04
    int chargeCash(char* data, unsigned char a, unsigned char b,
                   int encLen, char* encData, char* sendBuf,
                   char* wideBuf, char* more, int count,
                   NXTransaction& tx);                     // ORIG 0x081ade34

private:
    NCashImpl();
    NCashImpl(const NCashImpl&);
    NCashImpl& operator=(const NCashImpl&);

public:  // 成员偏移由 ORIG 使用点固定，暴露便于 TU 内访问器同型
    unsigned char   m_paramB;     // +0x00
    unsigned char   m_pad01;      // +0x01
    unsigned short  m_fieldC;     // +0x02
    short           m_fieldD;     // +0x04
    unsigned char   m_pad06[2];   // +0x06
    unsigned int    m_requestID;  // +0x08
    bool            m_connected;  // +0x0c
    char            m_serverAdrs[17]; // +0x0d..+0x1d
    unsigned short  m_port;       // +0x1e
    TCPSocket       m_socket;     // +0x20
    NCashMessage    m_inMsg;      // +0x40（0x208 区间含接收缓冲）
    NCashMessage    m_outMsg;     // +0x248..+0x450 outMsg 实体（ORIG 总 size 0x450）
};

// ---------------------------------------------------------------------------
// NexonCash —— NCashImpl 薄包装。唯一成员 NCashImpl* @+0。
// ctor new(0x450) 构造；dtor free(m_impl)（ORIG 汇编 free@plt 实证，
// 非 delete —— 忠实保留）。
// ---------------------------------------------------------------------------
class NexonCash
{
public:
    NexonCash(const char* serverAdrs, short port, char paramB,
              short fieldC, short fieldD);                   // ORIG 0x081ae1b0
    ~NexonCash();                                            // ORIG 0x081ae242

    int getBalance(const char* adrs, int& balance);          // ORIG 0x081ae258
    int checkEntrance(const char* adrs, char& out);          // ORIG 0x081ae4ae
    int purchase(unsigned long long sn, int productNo, int count,
                 const char* idStr, const char* extraId,
                 NXTransaction& tx);                         // ORIG 0x081ae27c
    int purchase(unsigned long long sn, int productNo, int count,
                 const char* idStr, const char* extraId,
                 NXTransaction& tx, short category);         // ORIG 0x081ae2ce
    int gift(unsigned long long sn, int productNo, int count,
             const char* idStr, const char* extraId,
             const char* recvName, const char* message,
             NXTransaction& tx);                             // ORIG 0x081ae330
    int confirm(const NXTransaction& tx);                    // ORIG 0x081ae390
    int cancel(const NXTransaction& tx);                     // ORIG 0x081ae3bc
    int charge(char* data, unsigned char a, unsigned char b,
               int encLen, char* encData, char* sendBuf,
               char* wideBuf, char* more, int count,
               NXTransaction& tx);                           // ORIG 0x081ae440
    int checkCoupon();                                       // ORIG 0x081ae3e8
    int useCoupon();                                         // ORIG 0x081ae3fe
    int checkCafeCoupon();                                   // ORIG 0x081ae414
    int useCafeCoupon();                                     // ORIG 0x081ae42a

private:
    NexonCash(const NexonCash&);
    NexonCash& operator=(const NexonCash&);

private:
    NCashImpl* m_impl;   // +0x00
};

}  // namespace cash
}  // namespace nexon

#endif  // DNF_NEXON_CASH_H
