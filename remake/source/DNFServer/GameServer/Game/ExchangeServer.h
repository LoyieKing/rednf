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

class CACHE_CHARACTER_TYPE;
template <typename T, int N> class StaticPool;

namespace exchange_server
{

const unsigned int HEADER_SIZE = 0x0d;   // ORIG r data 0x8c79790 = 0x0d

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

    int GetSocket();                     // ORIG 0x8490024 → base get_socket()
    void SetChannelNo(int channelNo);    // ORIG 0x848ffbe（+0x851a4）
    int GetChannelNo();                  // ORIG 0x848ffb0
    void SetTriggerSessionEventType(int type);  // ORIG 0x8490038（+0x851a0）
    void AddTriggerSessionEventType(int type);  // ORIG 0x849004a（|= ）
    int GetTriggerSessionEventType();           // ORIG 0x8490066

    int Parsing(int recvLen);            // ORIG 0x8490074（HEADER_SIZE 包解析）
    void OnRecv();                       // ORIG 0x849030e
    void OnSend();                       // ORIG 0x8490364
    void OnError();                      // ORIG 0x84903ee
    void OnAccept();                     // ORIG 0x849045e
    void OnConnect();                    // ORIG 0x84905d2
    int Send(PacketGuard& guard);        // ORIG 0x84906d0
    int Listen(int port, const char* ip);// ORIG 0x8490767
    int Connect(const char* ip, int port);  // ORIG 0x8490829
    void Disconnect();                   // ORIG 0x84908f2

private:
    unsigned int m_triggerEventType;     // +0x851a0
    unsigned int m_channelNo;            // +0x851a4
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
    int RegisterSession(SESSION* session, int fd);    // ORIG 0x84964ca
    void UnregisterSession(SESSION* session);         // ORIG 0x8496d5a
};

// ---------------------------------------------------------------------------
// CSessionManager<S,P,E> —— 仅基础类派生（ctor/dtor 仅调 E 子对象生命周期；
//   ORIG 尺寸 == sizeof(E)）。P=PacketGuard 当前未在方法签名外使用。
// ---------------------------------------------------------------------------
template <typename SESSION, typename PACKETGUARD, typename EPOLL>
class CSessionManager : public EPOLL
{
public:
    CSessionManager();
    ~CSessionManager();

    bool TryListen(int port, const char* ip);         // ORIG 0x8495570
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
    virtual bool DispatchPacket(CSession* session, char* data, int len) = 0;
    virtual bool UnregisterExchangeServer(int serverType) = 0;
    virtual CSession* AllocSession() = 0;
    virtual void FreeSession(CSession* session) = 0;
    virtual bool DispatchMessage(Stream* stream) = 0;

    IExchangeServer() {}
    ~IExchangeServer() {}                // 非虚空析构（ORIG 无 IES D 符号）
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
    virtual ~CExchangeServer();          // ORIG 0x8493b82

    void SetServerType(EXCHANGE_SERVER_TYPE type);   // ORIG 0x849103c（+0x68）
    EXCHANGE_SERVER_TYPE GetServerType();            // ORIG 0x8491030
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
    void UnregisterExchangeServer(int serverId);    // ORIG 0x849144c
    void SendRequestCharacInfo(int a, int b, int c, int d,
                               void* extra, int extraLen);  // ORIG 0x849152a
    void SendResponseCharacInfo(int a, int b, void* data, int len,
                                CSession* session, int f);  // ORIG 0x8491644
    void SendResponseCharacInfo(int a, int b, void* data, int len);  // 0x849173e
    void SendResponseCharacInfo_forchina(int a, int b, void* data,
                                         int len);           // ORIG 0x849176e
    void ReturnCacheCharacter(int a, int b, int c, int d, bool e,
                              CSession* session);           // ORIG 0x84918d8
    void ForwardRequestCharac_forchina(int a, int b, void* data,
                                       int len);            // ORIG 0x8491ab6
    void ForwardRequestCharac(int a, int b, void* data, int len);  // 0x8491c32
    void OnRecvPacketCharacInfo(CSession* session,
                                PacketGuard& guard);        // ORIG 0x84922ee
    void OnRecvPacketCharacInfoRet(CSession* session,
                                   PacketGuard& guard);     // ORIG 0x84925e8
    void OnRecvPacketServerInfo(CSession* session,
                                PacketGuard& guard);        // ORIG 0x8492854
    void OnRecvPacketServerInfoRet(CSession* session,
                                   PacketGuard& guard);     // ORIG 0x8492c5e
    bool DispatchPacket(CSession* session, char* data, int len);  // 0x8493002
    void OnRecvMessageServerInfo(Stream* stream, int a, int b);   // 0x84931de
    void OnRecvMessageCharacInfo(Stream* stream, int a, int b);   // 0x849341c
    bool DispatchMessage(Stream* stream);                         // 0x8493530
    Stream* GetMessage();                          // ORIG 0x849362a
    void FreeMessage(Stream* stream, int flag);    // ORIG 0x8493648
    void SendExchangeServerInfo();                 // ORIG 0x84937ba
    bool PreparePassiveSocket();                   // ORIG 0x8493856

private:
    char m_pad04[8];                     // +0x04..+0x0b（ORIG 未访问，语义未考证）
    CSessionManager<CSession, PacketGuard, CEpoll<CSession> >
                                         m_sessionMgr;    // +0x0c（sizeof 0x2c）
    bool m_flag38;                       // +0x38（SetMaster→1 / SetSlave→0）
    bool m_flag39;                       // +0x39（Reset 清零）
    unsigned int m_resetTime3c;          // +0x3c（Reset 置 time(0)）
    ExchangeServerInfo m_localInfo;      // +0x40
    std::map<int, ExchangeServerInfo> m_serverMap;   // +0x50（sizeof 0x18）
    int m_serverType68;                  // +0x68
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
unsigned short GetServerPORT();      // ORIG 0x848f2a3（返回宽度以报告为准）

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

private:
    unsigned int m_dbLoadCount;          // 计数字段（偏移见实现注释）
    unsigned int m_cacheLoadCount;
    unsigned int m_cacheLoadFailCount;
    unsigned int m_dbLoadOverflowCount;
    unsigned int m_cacheLoadOverflowCount;
    unsigned int m_cacheLoadFailOverflowCount;
};

CExchangeServerHitRate* GetInstanceExchangeServerHitRate();  // ORIG 0x823470f

// ---- 服务线程 ---------------------------------------------------------------
class CExchangeServerThread : public ThreadInterface
{
public:
    CExchangeServerThread();             // ORIG 0x8493e36
    virtual ~CExchangeServerThread();    // ORIG D1 0x8493db6 / D0 0x8493e18

    virtual void dispatch(void* arg);    // ORIG 0x8493bfe（覆盖 ThreadInterface 虚槽）

private:
    CExchangeServer m_exchangeServer;    // +0x0c
};

CExchangeServerThread* GetInstanceExchangeServerThread();   // ORIG 0x8493e8d

void BeginExchangeServerThread();        // ORIG 0x848f2b8（docs/exchange_server 报告）
unsigned long getExchangeServerThreadID();               // ORIG 0x848f2cd

}  // namespace exchange_server

#endif  // GAME_EXCHANGESERVER_H_
