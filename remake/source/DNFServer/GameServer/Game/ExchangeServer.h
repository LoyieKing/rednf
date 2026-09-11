#ifndef GAME_EXCHANGESERVER_H_
#define GAME_EXCHANGESERVER_H_

// ============================================================================
// df_game_r exchange_server 权威头（OnlyOrig_Exchange 批）
//
// 源自 /tmp/only_orig/exchange_server.txt 全部 84 个 ORIG-only 符号逐个反汇编
// （汇编快照 /tmp/es_disasm.txt），布局锚点：
//   CSession::C1        0x848ffd0：基类 CNetwork<95000,450000> 构造后清零
//                       +0x851a0(trigger 事件位集)/+0x851a4(channel no)。
//   IExchangeServer     vtable 0x8c7a1e8 全 __cxa_pure_virtual ×N（无虚析构，
//                       类内空 dtor 内联不产生符号）；C1 0x8493a88 仅写 vptr。
//   CExchangeServer C1  0x8493ae6：IExchangeServer C1 → 成员按序 C1：
//                       SessionManager@+0x0c、Info@+0x40、map@+0x50(0x18)、
//                       StaticPool<CSession,300>@+0x6c → Reset。
//   CCacheCharacterMgr  布局 Mutex@+0x00 / StaticPool@+0x18 /
//                       std::map<int,CACHE_CHARACTER_TYPE*>@+0x48。
//   Thread              CExchangeServerThread : ThreadInterface，
//                       +0x0c 内嵌 CExchangeServer，ctor 将其地址写入全局
//                       exchange_server_E(0x943d9f4)；vtable slot0=stop()(继承)、
//                       slot3=dispatch(void*)。
// ============================================================================

#include <map>

#include "CNetwork.h"         // CNetwork<95000,450000> 显式实例化声明
#include "EpollHandler.h"     // EpollHandler 基类（Mutex/Guard 同链引入）
#include "PacketGuard.h"
#include "Stream.h"
#include "ThreadInterface.h"

template <typename T, int N> class StaticPool;   // 仅指针/引用成员需要；
class CACHE_CHARACTER_TYPE;  // 前置：CCacheCharacterMgr 接口用；完整类型 CacheCharacterType.h
                                                // m_sessionPool/m_pool 完整类型由
#include "StaticPool.h"                         // 包含 TU 隐式实例化使用。

namespace exchange_server
{

// [WorldLane 2026-09-11 · static 多副本普查] 原 `const unsigned int HEADER_SIZE = 0x0d;`
// 已下沉到唯一使用 TU `ExchangeServer_Session.cpp`（ORIG `_ZN15exchange_serverL11HEADER_SIZEE`
// 为单份 r 4B @0x08c79790，仅 1 TU 发射；头内定义会随 include 链在 71 个 TU 各发射一份）。
class IExchangeServer;   // 前置：CSession::exchange_server_ 静态成员用（定义见下）

enum EXCHANGE_SERVER_TYPE
{
    EXCHANGE_SERVER_MASTER = 0,          // SetMaster 传 0（ORIG 0x849114c）
    EXCHANGE_SERVER_SLAVE  = 1           // SetSlave  传 1（ORIG 0x84911a4）
};

enum CONNECT_TYPE
{
    // 数值与语义未考证（worklist 中仅作 GetExchangeServerCount 形参类型参与
    // mangling；运行时只与 ExchangeServerInfo 存储值做整型比较）。
    EXCHANGE_CONNECT_UNKNOWN = 0
};

// ---------------------------------------------------------------------------
// CSession —— 会话基座（ORIG sizeof = 基类 0x851a0 + 8）
// ---------------------------------------------------------------------------
class CSession : public CNetwork<95000, 450000>
{
public:
    CSession();                          // ORIG 0x848ffd0
    ~CSession();                         // ORIG D1 0x8495662（仅基类析构）

    int GetSocket();                     // ORIG 0x8490024 → base get_socket()
    void SetChannelNo(int channelNo);    // ORIG 0x848ffbe（+0x851a4）
    int GetChannelNo();                  // ORIG 0x848ffb0
    void SetTriggerSessionEventType(int type);  // ORIG 0x8490038（+0x851a0）
    void AddTriggerSessionEventType(int type);  // ORIG 0x849004a（|= ）
    int GetTriggerSessionEventType();           // ORIG 0x8490066
    static IExchangeServer* exchange_server_;   // ORIG W _ZN15exchange_server8CSession16exchange_server_E
    CSession* OnAccept();                // ORIG 0x849045e：accept+set_socket 后返回新会话
    bool OnConnect();                    // ORIG 0x84905d2
    bool OnRecv();                       // ORIG 0x849030e
    int OnSend();                        // ORIG 0x8490364（-1=失败）
    bool OnError();                      // ORIG 0x84903ee
    bool Send(PacketGuard& guard);       // ORIG 0x84906d0（PRETTY 串 "bool ...Send(PacketGuard&)" 实证）
    bool Parsing(int recvLen);                   // ORIG 0x8490074（HEADER_SIZE 包解析；
                                                 // SIGFIX: ORIG 串 "bool ...Parsing(int)"）
    // ORIG 两函数调用约定未传 this（TryListen/TryConnect 仅压 port/ip），
    // 实为静态成员；ORIG 返回寄存器承载新分配的 session 指针（0=失败）。
    static CSession* Listen(int port, const char* ip);   // ORIG 0x8490767
    static CSession* Connect(const char* ip, int port);  // ORIG 0x8490829
    bool Disconnect();                   // ORIG 0x84908f2（P159：尾 mov$0x1,%eax 判 bool）

private:
    int m_triggerEventType;              // +0x851a0（int：形参 Ei 同型直赋，
                                         // 4.4 无转换赋值 LHS 先=ORIG 实证）
    int m_channelNo;                     // +0x851a4（同上）
};

// ---------------------------------------------------------------------------
// CEpoll —— 事件轮询（模板对 SESSION 单参实例化；ORIG 符号仅 CSession 实例化：
//   _ZN15exchange_server6CEpollINS_8CSessionEE{C1,C2,D0,D1,D2,
//   WaitForEvent,RegisterSession,UnregisterSession}）
// 方法实现见 ExchangeServer_Infra.cpp 的显式实例化区。
// ---------------------------------------------------------------------------
template <typename SESSION>
class CEpoll : public EpollHandler
{
public:
    CEpoll();
    ~CEpoll();

    bool WaitForEvent(int timeoutMs);                 // ORIG 0x8495f0a
    bool RegisterSession(SESSION* session, int fd);   // ORIG 0x84964ca（4.1 sete%al 无规范化）
    bool UnregisterSession(SESSION* session);         // ORIG 0x8496d5a（返 !ResetEpoll 物化）
};

// ---------------------------------------------------------------------------
// CSessionManager<S,P,E> —— 仅基础类派生（ctor/dtor 仅调 E 子对象生命周期；
//   ORIG 尺寸 == sizeof(E)）。P=PacketGuard 当前未在方法签名外使用。
// ---------------------------------------------------------------------------
template <typename SESSION, typename PACKETGUARD,
          template <typename> class EPOLL>   // template-template 参数（ORIG mangled
                                             // 第三实参为无实参模板名，实证）
class CSessionManager : public EPOLL<SESSION>
{
public:
    CSessionManager();
    ~CSessionManager();

    bool TryListen(int port, const char* ip);         // ORIG 0x8495570（尾直通不规范化；消费点 xor/test bool 链）
    bool TryConnect(const char* ip, int port);        // ORIG 0x8495506
    void TryDisconnect(SESSION* session);             // ORIG 0x8496236
    bool TrySend(SESSION* session, PACKETGUARD& g);   // ORIG 0x84953b2
    bool WaitForSessionEvent(int timeoutMs);          // ORIG 0x84952e0
};

class ExchangeServerInfo;

// ---------------------------------------------------------------------------
// IExchangeServer —— 抽象接口（vtable 0x8c7a1e8 全纯虚；具体纯虚集合
// [推断]：由 CExchangeServer vtable 前缀槽位逆推，均被派生覆盖；无虚析构）
// ---------------------------------------------------------------------------
class IExchangeServer
{
public:
    // ORIG 0x8493002 尾 `mov %ebx,%eax` 无 bool 归一（0x315 直传）⇒ int 返回；
    // ORIG 0x849144c 成功 `mov $0x1,%eax` / 失败 `mov $0x0,%eax` ⇒ bool 返回。
    virtual int DispatchPacket(CSession* session, char* data, int len) = 0;
    virtual bool UnregisterExchangeServer(int serverId) = 0;
    virtual CSession* AllocSession() = 0;
    virtual void FreeSession(CSession* session) = 0;
    // vtable 0x8c7a1d0 第 5 槽实证 = GetServerType（OnRecvPacketServerInfo*
    // 存在 this->vtable[4]() 无参虚调用）。DispatchMessage 为非虚成员。
    virtual EXCHANGE_SERVER_TYPE GetServerType() = 0;

    IExchangeServer() {}
    // ORIG 无 ~IExchangeServer 符号：析构为隐式 trivial（派生 D1 不生成基类
    // 析构调用；ORIG vtable 亦无虚析构槽）。不得显式声明析构。
};

struct ExchangeServerInfo
{
    ExchangeServerInfo();                // ORIG 0x8491004

    int m_field00;                       // +0x00（ORIG 清零，语义未考证）
    unsigned short m_word04;             // +0x04
    CONNECT_TYPE m_type08;               // +0x08（GetExchangeServerCount 比较域）
    CSession* m_pSession;                // +0x0c（AttachSessionInExchangeServer 写入）
};                                       // sizeof 0x10

// ---------------------------------------------------------------------------
// CExchangeServer —— 本体
// ---------------------------------------------------------------------------
class CExchangeServer : public IExchangeServer
{
public:
    CExchangeServer();                   // ORIG 0x8493ae6
    ~CExchangeServer();                  // ORIG 0x8493b82：非虚（ORIG 仅 D1 符号，无 D0；
                                          // virtual 会产生 D0 + gcc 4.4 D1 尾部 if(0) delete 处理）

    void SetServerType(EXCHANGE_SERVER_TYPE type);   // ORIG 0x849103c（+0x68）
    EXCHANGE_SERVER_TYPE GetServerType();            // ORIG 0x8491030（覆盖基类纯虚）
    CSession* AllocSession();                        // ORIG 0x849104a（池 Acquire）
    void FreeSession(CSession* session);             // ORIG 0x8491060
    void Reset();                                    // ORIG 0x849107e
    bool GetNetworkEvent();                          // ORIG 0x84910a6（等待 0xa ms）
    int GetExchangeServerCount(CONNECT_TYPE type);   // ORIG 0x84910ca
    void SetMaster();                                // ORIG 0x849114c
    void SetSlave();                                 // ORIG 0x84911a4
    bool TrySend(int serverId, PacketGuard& guard);  // ORIG 0x84911fc
    bool AttachSessionInExchangeServer(int serverId,
                                       CSession* session);  // ORIG 0x8491296
    bool RegisterExchangeServer(int serverId,
                                ExchangeServerInfo& info);  // ORIG 0x849135c
    bool UnregisterExchangeServer(int serverId);     // ORIG 0x849144c（bool：mov $1/$0）
    bool SendRequestCharacInfo(int a, int b, int c, int d,
                               void* extra, int extraLen);  // ORIG 0x849152a
    bool SendResponseCharacInfo(int a, int b, void* data, int len,
                                CSession* session, int f);  // ORIG 0x8491644
    bool SendResponseCharacInfo(int a, int b, void* data, int len);  // 0x849173e
    bool SendResponseCharacInfo_forchina(int a, int b, void* data,
                                         int len);           // ORIG 0x849176e
    bool ReturnCacheCharacter(int a, int b, int c, int d, bool e,
                              CSession* session);           // ORIG 0x84918d8
    bool ForwardRequestCharac_forchina(int a, int b, void* data,
                                       int len);            // ORIG 0x8491ab6
    bool ForwardRequestCharac(int a, int b, void* data, int len);  // 0x8491c32
    int OnRecvPacketCharacInfo(CSession* session,
                               PacketGuard& guard);        // ORIG 0x84922ee
    int OnRecvPacketCharacInfoRet(CSession* session,
                                  PacketGuard& guard);     // ORIG 0x84925e8
    int OnRecvPacketServerInfo(CSession* session,
                               PacketGuard& guard);        // ORIG 0x8492854
    int OnRecvPacketServerInfoRet(CSession* session,
                                  PacketGuard& guard);     // ORIG 0x8492c5e
    int DispatchPacket(CSession* session, char* data, int len);   // 0x8493002（int：0x315 直传）
    int OnRecvMessageServerInfo(Stream* stream, int a, int b);    // 0x84931de
                                                 // SIGFIX: ORIG 串 "int ...OnRecvMessageServerInfo(Stream*, int, int)"
    int OnRecvMessageCharacInfo(Stream* stream, int a, int b);    // 0x849341c
    bool DispatchMessage(Stream* stream);                         // 0x8493530
    Stream* GetMessage();                          // ORIG 0x849362a
    bool FreeMessage(Stream* stream, int flag);    // ORIG 0x8493648
    bool SendExchangeServerInfo();                 // ORIG 0x84937ba（dispatch 消费返回值）
    bool PreparePassiveSocket();                   // ORIG 0x8493856

private:
    int m_ip04;                          // +0x04（ORIG `mov 0x4(%eax),%esi` 单段直落 ⇒ 与 int 实参同型）
    unsigned short m_port08;             // +0x08
    char m_pad0a[2];                     // 补齐至 +0x0c
    CSessionManager<CSession, PacketGuard, CEpoll>
                                         m_sessionMgr;    // +0x0c（sizeof 0x2c）
    bool m_flag38;                       // +0x38（SetMaster→1 / SetSlave→0）
    bool m_flag39;                       // +0x39（Reset 清零）
    long m_resetTime3c;                  // +0x3c（Reset 置 time(0)；time_t 同型直存，
                                         //   实证 /tmp/l11_n92.cpp：4.4 对 long 字段
                                         //   `mov this,%edx; mov %eax,(%edx)` 与 ORIG 同形，
                                         //   uint 字段则恒多 mov%eax,%edx 中转=N92 真身）
    ExchangeServerInfo m_localInfo;      // +0x40
    std::map<int, ExchangeServerInfo> m_serverMap;   // +0x50（sizeof 0x18）
    EXCHANGE_SERVER_TYPE m_serverType68;  // +0x68（枚举字段：SetServerType 枚举
                                          // 直赋无转换=ORIG LHS 先实证；宽度同 int）
    StaticPool<CSession, 300> m_sessionPool;          // +0x6c
};

// ---------------------------------------------------------------------------
// CCacheCharacterMgr —— 角色缓存管理器（单例经 GetInstanceCacheCharacterMgr）
// ---------------------------------------------------------------------------
class CCacheCharacterMgr
{
public:
    CCacheCharacterMgr();                // ORIG 0x8490e98
    ~CCacheCharacterMgr();               // ORIG 0x8490f04

    CACHE_CHARACTER_TYPE* AcquireCharacter(int characNo);   // 0x8490a8a
    void FreeCharacter(CACHE_CHARACTER_TYPE* mem);          // 0x8490bf4
    bool CacheCharacter(int characNo,
                        CACHE_CHARACTER_TYPE* mem);         // 0x8490dca
    bool GetCacheCharacter(int characNo, CACHE_CHARACTER_TYPE** out,
                           bool erase);                     // 0x8490cee
    bool CollectGarbage();                                  // 0x8490992

private:
    Mutex m_mutex;                                        // +0x00
    StaticPool<CACHE_CHARACTER_TYPE, 1000> m_pool;        // +0x18
    std::map<int, CACHE_CHARACTER_TYPE*> m_map;           // +0x48
};

CCacheCharacterMgr* GetInstanceCacheCharacterMgr();       // ORIG 0x8490f71

CACHE_CHARACTER_TYPE* AcquireCharacterMemory(int characNo);       // 0x848f220
void FreeCharacterMemory(CACHE_CHARACTER_TYPE* mem);              // 0x848f23c
bool CacheCharacterMemory(int characNo, CACHE_CHARACTER_TYPE* mem);// 0x848f258

int GetServerChannelNo();            // ORIG 0x848f27b
char* GetServerIP();                 // ORIG 0x848f28e
int GetServerPORT();                 // ORIG 0x848f2a3（调用点直取 eax 无 movzwl 实证 int 返回）

// ---- 命中率统计 ------------------------------------------------------------
class CExchangeServerHitRate
{
public:
    CExchangeServerHitRate();            // ORIG 0x82346fc
    void Reset();                        // ORIG 0x82346bc

    void IncrementDBLoadCount();         // ORIG 0x823465a
    void IncrementCacheLoadCount();      // ORIG 0x823468a
    void IncrementCacheLoadFailCount();  // ORIG 0x848f35e

    int GetDBLoadCount();                // ORIG 0x863bfb0
    int GetCacheLoadCount();             // ORIG 0x863bfba
    int GetDBLoadOverflowCount();        // ORIG 0x863bfc6
    int GetCacheLoadOverflowCount();     // ORIG 0x863bfd2
    int GetCacheLoadFailCount();         // ORIG 0x863bfde
    int GetCacheLoadFailOverflowCount(); // ORIG 0x863bfea

    // 六字段 ORIG 偏移序（/tmp/es_disasm.txt:680-867、5591-5632 逐指令核实）：
    //   +0x00 dbLoad / +0x04 cacheLoad / +0x08 dbLoadOverflow /
    //   +0x0c cacheLoadOverflow / +0x10 cacheLoadFail / +0x14 cacheLoadFailOverflow
    // Increment*: 计数自增后若归零(回绕)则对应 Overflow 槽 +1。
    unsigned int m_dbLoadCount;                 // +0x00
    unsigned int m_cacheLoadCount;              // +0x04
    unsigned int m_dbLoadOverflowCount;         // +0x08（对应计数回绕为 0 时自增）
    unsigned int m_cacheLoadOverflowCount;      // +0x0c
    unsigned int m_cacheLoadFailCount;          // +0x10
    unsigned int m_cacheLoadFailOverflowCount;  // +0x14
};

CExchangeServerHitRate* GetInstanceExchangeServerHitRate();  // ORIG 0x823470f


class CExchangeServerThread : public ThreadInterface
{
public:
    CExchangeServerThread();             // ORIG C1 0x8493e36
    virtual ~CExchangeServerThread();    // ORIG D1 0x8493db6 / D0 0x8493e18

    virtual void dispatch(void* arg);    // ORIG 0x8493bfe（覆盖 ThreadInterface 虚槽）

private:
    CExchangeServer m_exchangeServer;    // +0x0c
};

CExchangeServerThread* GetInstanceExchangeServerThread();   // ORIG 0x8493e8d

// [b28d-Main 代落·S249 P292] void→bool：ORIG 0x848f2b8 透传 begin() 的 %eax；
// 调用点 0x0829ffdb `call; mov %al,-0x9` 存字节 ⇒ 返回 bool。返回类型不入 mangled。
bool BeginExchangeServerThread();        // ORIG 0x848f2b8（docs/exchange_server 报告）
unsigned long getExchangeServerThreadID();               // ORIG 0x848f2cd

}  // namespace exchange_server

#endif  // GAME_EXCHANGESERVER_H_
