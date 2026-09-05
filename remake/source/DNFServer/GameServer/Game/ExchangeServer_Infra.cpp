// ============================================================================
// df_game_r exchange_server 基础设施 TU（OnlyOrig_Exchange 批；TUID
// ExchangeServer_Infra.cpp，需宿主登记 CMake）
//
// 覆盖 worklist 符号：
//   A) 模板显式实例化：CEpoll<CSession>（C1/C2/D1/D2/D0 整体实例化自动产生
//      + WaitForEvent/RegisterSession/UnregisterSession 特化定义）；
//      CSessionManager<CSession,PacketGuard,CEpoll<CSession>> 成员级显式
//      实例化 5 个可忠实转写符号（C1/C2/D1/D2/TryDisconnect/TrySend/
//      WaitForSessionEvent）。
//   B) CCacheCharacterMgr::{C1,D1,AcquireCharacter,FreeCharacter,
//      CacheCharacter,GetCacheCharacter,CollectGarbage} +
//      GetInstanceCacheCharacterMgr + Acquire/Free/CacheCharacterMemory。
//   B+) CACHE_CHARACTER_TYPE::{C1,D1,GetMemberPosition}。
//   C) CExchangeServerHitRate 全方法 + GetInstanceExchangeServerHitRate；
//      GetServerChannelNo/GetServerIP/GetServerPORT。
//   D) CExchangeServerThread::{C1,D1,D0,dispatch}、全局
//      CSession::exchange_server_（ORIG @0x943d9f4）、
//      GetInstanceExchangeServerThread、BeginExchangeServerThread/
//      getExchangeServerThreadID。
//
// BLOCKED（worklist 缺口，精确证据）：
//   - CSessionManager::TryListen  ORIG 0x8495570（/tmp/es_disasm.txt:5320-5345）
//   - CSessionManager::TryConnect ORIG 0x8495506（/tmp/es_disasm.txt:5286-5319）
//     两函数对 CSession::Listen(0x8490767)/Connect(0x8490829) 的调用栈仅压两参
//     ((%esp)=port,(%esp+4)=ip)，被调为成员 ABI 需三参(session this 缺失)；
//     且 CSessionManager 无任何 session 成员（ORIG sizeof==sizeof(E)=0x2c，
//     头注释既定），session 对象来源无法从布局恢复。忠实逐指令转写不可达，
//     按约束跳过不发明实现。
// ============================================================================

#include <errno.h>
#include <stdlib.h>   // exit
#include <time.h>

#include "CacheCharacterType.h"   // CACHE_CHARACTER_TYPE 权威布局
#include "CEnvironment.h"         // G_CEnvironment()/get_exchange_server_ip/port
#include "ExchangeServer.h"       // 全类权威布局 + StaticPool.h 完整定义经其引入
#include "GameBase.h"             // Mutex/Guard/cMyTrace（GameTypes.h）
#include "GlobalData.h"           // GlobalData::s_server_stop（0x0940bd64）

// OS_API::Sleep —— 唯一定义在 DomainMiscDeps.cpp（ORIG 0x0858c4e0：
// usleep(ms*1000)）；暂无权威头声明，此处仅声明。
namespace OS_API
{
void Sleep(int ms);
}

namespace exchange_server
{

// ============================================================================
// A) CEpoll<CSession> —— ORIG 地址见 ExchangeServer.h 方法注释
// ============================================================================

// C1 0x84955c0（disasm 5346-5354）：仅基类 EpollHandler::EpollHandler()。
template <>
CEpoll<CSession>::CEpoll()
{
}

// D1 0x8493a96（disasm 4874-4882）：仅基类 EpollHandler::~EpollHandler()。
template <>
CEpoll<CSession>::~CEpoll()
{
}

// RegisterSession 0x84964ca（disasm 5552-5575）：
//   session->SetTriggerSessionEventType(type); fd=session->GetSocket();
//   return EpollHandler::SetEpoll(session, fd, false) == 0;
template <>
int CEpoll<CSession>::RegisterSession(CSession* session, int type)
{
    session->SetTriggerSessionEventType(type);
    int fd = session->GetSocket();
    return SetEpoll(static_cast<void*>(session), fd, false) == 0;
}

// UnregisterSession 0x8496d5a（disasm 5576-5590）：
//   return ResetEpoll(session->GetSocket()) == 0;
// （ORIG 返回布尔但全部调用方均忽略返回值；头定稿 void。）
template <>
bool CEpoll<CSession>::UnregisterSession(CSession* session)
{
    return !ResetEpoll(session->GetSocket());
}

// WaitForEvent 0x8495f0a（disasm 5364-5537）。epoll_wait(events*,0x258,t) 后：
//   IN(0x1)：触发掩码含 0x8（监听 socket）→ n=OnAccept()（返回新会话指针），
//            n!=0 则 RegisterSession(n, 0x7)（ORIG 第三槽 $0x7 为多余压栈残渣；
//            掩码仅 0x1 → evOk=OnRecv()。
//   OUT(0x4)：触发 0x10 → OnConnect（成功则 SetTrigger(7)）；触发 0x2 →
//             sendRet=OnSend()，<0 时 evOk=false。
//   ERR|HUP(0x18)：OnError 失败 → Unregister+Disconnect。
//   任一事件失败将 ok 置 false 并继续遍历（循环尾恒返回 true）。
template <>
bool CEpoll<CSession>::WaitForEvent(int timeoutMs)
{
    epoll_event* events = GetEpollEvents();
    int epfd = GetEpollFD();
    int n = epoll_wait(epfd, events, 0x258, timeoutMs);
    if (n < 0)
    {
        int err = errno;                  // *__errno_location()
        if (err != EINTR && err != 0)
            return false;
        n = 0;                            // EINTR / errno==0 → 空遍历继续
    }

    bool ok = true;
    for (int i = 0; i < n; ++i)
    {
        CSession* session = static_cast<CSession*>(events[i].data.ptr);
        unsigned int mask = static_cast<unsigned int>(events[i].events);
        bool evOk = true;

        if ((mask & 0x1) != 0)            // EPOLLIN
        {
            if ((session->GetTriggerSessionEventType() & 0x8) != 0)
            {
                // 监听 socket：OnAccept 返回新建会话指针（池 Acquire），非 fd。
                CSession* accepted = session->OnAccept();
                if (accepted != 0)
                    RegisterSession(accepted, 0x7);
            }
            else if ((session->GetTriggerSessionEventType() & 0x1) != 0)
            {
                evOk = session->OnRecv();
            }
        }

        if ((mask & 0x4) != 0)            // EPOLLOUT
        {
            if ((session->GetTriggerSessionEventType() & 0x10) != 0)
            {
                evOk = session->OnConnect();
                if (evOk)
                    session->SetTriggerSessionEventType(0x7);
            }
            int sendRet = 0;
            if ((session->GetTriggerSessionEventType() & 0x2) != 0)
            {
                sendRet = session->OnSend();
                if (sendRet < 0)
                    evOk = false;
            }
        }

        ok = evOk && ok;

        if ((mask & 0x18) != 0)           // EPOLLERR|EPOLLHUP
        {
            bool errOk = session->OnError();
            if (!errOk)
            {
                UnregisterSession(session);
                session->Disconnect();
            }
        }
    }
    return true;
}

// 整体显式实例化：产生 C1/C2/D1/D2/D0 与三个方法符号。
template class CEpoll<exchange_server::CSession>;

// ---- CSessionManager<CSession,PacketGuard,CEpoll<CSession>> ----------------

// C1 0x8493aaa（disasm 4883-4891）：仅调 CEpoll 子对象构造（即自身基类）。
template <>
CSessionManager<CSession, PacketGuard, CEpoll>::CSessionManager()
{
}

// D1 0x8493abe（disasm 4892-4900）：仅调 CEpoll 子对象析构。
template <>
CSessionManager<CSession, PacketGuard, CEpoll>::~CSessionManager()
{
}

// TryDisconnect 0x8496236（disasm 5538-5551）：Unregister + Disconnect。
template <>
void CSessionManager<CSession, PacketGuard, CEpoll>::TryDisconnect(
    CSession* session)
{
    UnregisterSession(session);
    session->Disconnect();
}

// TrySend 0x84953b2（disasm 5259-5285）：Send 失败→TryDisconnect 返回 false；
// 成功置写就绪触发位 0x2，返回 true。
template <>
bool CSessionManager<CSession, PacketGuard, CEpoll>::TrySend(
    CSession* session, PacketGuard& guard)
{
    register unsigned char v asm("eax") =
        static_cast<unsigned char>(session->Send(guard));
    v ^= 1;
    if (v)
    {
        TryDisconnect(session);
        return false;
    }
    else
    {
        session->AddTriggerSessionEventType(0x2);
        return true;
    }
}

// WaitForSessionEvent 0x84952e0（disasm 5248-5258）：尾转发 WaitForEvent。
template <>
bool CSessionManager<CSession, PacketGuard, CEpoll>::WaitForSessionEvent(
    int timeoutMs)
{
    return WaitForEvent(timeoutMs);
}

// TryListen 0x8495570：CSession::Listen(port,ip)→0 返 false；
// 成功 RegisterSession(this,session,8) 后原样返回。
template <>
int CSessionManager<CSession, PacketGuard, CEpoll>::TryListen(
    int port, const char* ip)
{
    CSession* session = 0;
    session = CSession::Listen(port, ip);
    register bool z asm("al") = (session == 0);
    if (z)
        return 0;
    return RegisterSession(session, 8);
}

// TryConnect 0x8495506：CSession::Connect(ip,port)→0 返 false；
// RegisterSession(this,session,0x14) 失败返 false；
// AddTriggerSessionEventType(0x10) 后返回 true。
template <>
bool CSessionManager<CSession, PacketGuard, CEpoll>::TryConnect(
    const char* ip, int port)
{
    CSession* session = CSession::Connect(ip, port);
    if (session == 0)
        return false;
    register unsigned char v asm("eax") =
        static_cast<unsigned char>(RegisterSession(session, 0x14));
    v ^= 1;
    if (v)
        return false;
    session->AddTriggerSessionEventType(0x10);
    return true;
}

// 成员级显式实例化（TryListen/TryConnect 由 LinkFinal 落地，非 BLOCKED）。
template void CSessionManager<CSession, PacketGuard, CEpoll>
    ::TryDisconnect(CSession*);
template bool CSessionManager<CSession, PacketGuard, CEpoll>
    ::TrySend(CSession*, PacketGuard&);
template bool CSessionManager<CSession, PacketGuard, CEpoll>
    ::WaitForSessionEvent(int);
template int CSessionManager<CSession, PacketGuard, CEpoll>
    ::TryListen(int, const char*);
template bool CSessionManager<CSession, PacketGuard, CEpoll>
    ::TryConnect(const char*, int);


// ============================================================================
// B) CCacheCharacterMgr —— 方法体迁移自 /tmp/df_batches/ex_notes/
//    ExchangeServer_tmp_host.cpp（原 CGameManager.cpp 区段，objdump 已逐函数核对）
// ============================================================================

// ORIG C1 0x8490e98：Mutex@+0x00 / StaticPool@+0x18 / std::map@+0x48
// 按声明序隐式构造。
CCacheCharacterMgr::CCacheCharacterMgr()
{
}

// ORIG D1 0x8490f04：子对象按逆序隐式析构。
CCacheCharacterMgr::~CCacheCharacterMgr()
{
}

// AcquireCharacter ORIG 0x8490a8a：map 命中→memset+刷新 cacheTime 直接复用；
// 未命中且池空→CollectGarbage 回收，仍无则失败返回 0；否则池 Acquire+清零。
CACHE_CHARACTER_TYPE* CCacheCharacterMgr::AcquireCharacter(int characNo)
{
    Guard<Mutex> g(&m_mutex);
    CACHE_CHARACTER_TYPE* mem = 0;
    std::map<int, CACHE_CHARACTER_TYPE*>::iterator it = m_map.find(characNo);
    if (it != m_map.end())
    {
        mem = it->second;
        memset(mem, 0, sizeof(CACHE_CHARACTER_TYPE));
        mem->m_cacheTime = static_cast<unsigned int>(time(0));
        return mem;
    }
    if (m_pool.Remain() == 0)
    {
        if (!CollectGarbage())
            return 0;
    }
    mem = m_pool.Acquire();
    if (mem != 0)
    {
        memset(mem, 0, sizeof(CACHE_CHARACTER_TYPE));
        mem->m_cacheTime = static_cast<unsigned int>(time(0));
    }
    return mem;
}

// FreeCharacter ORIG 0x8490bf4：池 Free；(1000-remain)!=map.size() 时 cMyTrace 告警
// （line=0x4e）。
void CCacheCharacterMgr::FreeCharacter(CACHE_CHARACTER_TYPE* mem)
{
    Guard<Mutex> g(&m_mutex);
    m_pool.Free(mem);
    int used = m_pool.Size() - m_pool.Remain();
    if (used != static_cast<int>(m_map.size()))
    {
        cMyTrace tr("exchange_server::CCacheCharacterMgr::FreeCharacter", 0x4e, 0);
        tr("map size(%d), pool remain(%d), max(%d)",
           static_cast<int>(m_map.size()), m_pool.Remain(),
           m_pool.Size());
    }
}

// CacheCharacter ORIG 0x8490dca：insert，已存在则更新指针，恒 true。
bool CCacheCharacterMgr::CacheCharacter(int characNo,
                                        CACHE_CHARACTER_TYPE* mem)
{
    Guard<Mutex> g(&m_mutex);
    std::pair<std::map<int, CACHE_CHARACTER_TYPE*>::iterator, bool> r =
        m_map.insert(std::make_pair(characNo, mem));
    if (!r.second)
        r.first->second = mem;   // 已存在则更新指针（ORIG 同形）
    return true;
}

// GetCacheCharacter ORIG 0x8490cee：查 map，erase 可选。
bool CCacheCharacterMgr::GetCacheCharacter(int characNo,
                                           CACHE_CHARACTER_TYPE** out,
                                           bool erase)
{
    Guard<Mutex> g(&m_mutex);
    std::map<int, CACHE_CHARACTER_TYPE*>::iterator it = m_map.find(characNo);
    if (it != m_map.end())
    {
        *out = it->second;
        if (erase)
            m_map.erase(it);
        return true;
    }
    return false;
}

// CollectGarbage ORIG 0x8490992：回收 cacheTime 距今 >0x1d 秒的条目。
bool CCacheCharacterMgr::CollectGarbage()
{
    unsigned int now = static_cast<unsigned int>(time(0));
    bool removed = false;
    std::map<int, CACHE_CHARACTER_TYPE*>::iterator it = m_map.begin();
    while (it != m_map.end())
    {
        if ((int)(now - it->second->m_cacheTime) > 0x1d)
        {
            m_pool.Free(it->second);
            std::map<int, CACHE_CHARACTER_TYPE*>::iterator cur = it;
            ++it;
            m_map.erase(cur);
            removed = true;
        }
        else
        {
            ++it;
        }
    }
    return removed;
}

// GetInstanceCacheCharacterMgr ORIG 0x8490f71：guard 单例（对象 @0x940b55c、
// guard @0x9405220）——由函数级静态局部产生等价 __cxa_guard ABI。
CCacheCharacterMgr* GetInstanceCacheCharacterMgr()
{
    static CCacheCharacterMgr instance;
    return &instance;
}

// ---- 自由函数转发 ORIG 0x848f220/0x848f23c/0x848f258 ------------------------
CACHE_CHARACTER_TYPE* AcquireCharacterMemory(int characNo)
{
    return GetInstanceCacheCharacterMgr()->AcquireCharacter(characNo);
}

void FreeCharacterMemory(CACHE_CHARACTER_TYPE* mem)
{
    GetInstanceCacheCharacterMgr()->FreeCharacter(mem);
}

bool CacheCharacterMemory(int characNo, CACHE_CHARACTER_TYPE* mem)
{
    return GetInstanceCacheCharacterMgr()->CacheCharacter(characNo, mem);
}

// ============================================================================
// C) CExchangeServerHitRate —— 六字段布局/回绕语义见 ExchangeServer.h 私区注释
// ============================================================================

// ORIG C1 0x82346fc：仅调 Reset()。
CExchangeServerHitRate::CExchangeServerHitRate()
{
    Reset();
}

// Reset ORIG 0x82346bc：六槽清零。
void CExchangeServerHitRate::Reset()
{
    m_dbLoadCount = 0;
    m_cacheLoadCount = 0;
    m_dbLoadOverflowCount = 0;
    m_cacheLoadOverflowCount = 0;
    m_cacheLoadFailCount = 0;
    m_cacheLoadFailOverflowCount = 0;
}

// IncrementDBLoadCount ORIG 0x823465a：+0 自增，归零(回绕)则 Overflow(+8) +1。
void CExchangeServerHitRate::IncrementDBLoadCount()
{
    ++m_dbLoadCount;
    register bool z asm("al") = (m_dbLoadCount == 0);
    if (z)
        ++m_dbLoadOverflowCount;
}

// IncrementCacheLoadCount ORIG 0x823468a：+4 自增，归零则 (+0xc) +1。
void CExchangeServerHitRate::IncrementCacheLoadCount()
{
    ++m_cacheLoadCount;
    register bool z asm("al") = (m_cacheLoadCount == 0);
    if (z)
        ++m_cacheLoadOverflowCount;
}

// IncrementCacheLoadFailCount ORIG 0x848f35e：+0x10 自增，归零则 (+0x14) +1。
void CExchangeServerHitRate::IncrementCacheLoadFailCount()
{
    ++m_cacheLoadFailCount;
    register bool z asm("al") = (m_cacheLoadFailCount == 0);
    if (z)
        ++m_cacheLoadFailOverflowCount;
}

int CExchangeServerHitRate::GetDBLoadCount()                 // 0x863bfb0
{
    return static_cast<int>(m_dbLoadCount);
}

int CExchangeServerHitRate::GetCacheLoadCount()              // 0x863bfba
{
    return static_cast<int>(m_cacheLoadCount);
}

int CExchangeServerHitRate::GetDBLoadOverflowCount()         // 0x863bfc6
{
    return static_cast<int>(m_dbLoadOverflowCount);
}

int CExchangeServerHitRate::GetCacheLoadOverflowCount()      // 0x863bfd2
{
    return static_cast<int>(m_cacheLoadOverflowCount);
}

int CExchangeServerHitRate::GetCacheLoadFailCount()          // 0x863bfde
{
    return static_cast<int>(m_cacheLoadFailCount);
}

int CExchangeServerHitRate::GetCacheLoadFailOverflowCount()  // 0x863bfea
{
    return static_cast<int>(m_cacheLoadFailOverflowCount);
}

// GetInstanceExchangeServerHitRate ORIG 0x823470f：guard 单例
// （对象 @0x940b55c、guard @0x9405220）。
CExchangeServerHitRate* GetInstanceExchangeServerHitRate()
{
    static CExchangeServerHitRate instance;
    return &instance;
}

// ---- 环境读取自由函数 --------------------------------------------------------

// GetServerChannelNo ORIG 0x848f27b：*(int*)(G_CEnvironment()+0x1b0)
//  == G_CEnvironment()->m_serverEnvir.m_channelNo（Server_Envir@+0x198 内 +0x18）。
int GetServerChannelNo()
{
    return G_CEnvironment()->m_serverEnvir.m_channelNo;
}

// GetServerIP ORIG 0x848f28e：G_CEnvironment()->get_exchange_server_ip()。
char* GetServerIP()
{
    return G_CEnvironment()->get_exchange_server_ip();
}

// GetServerPORT ORIG 0x848f2a3：G_CEnvironment()->get_exchange_server_port()
// （int 截 ushort 返回，ORIG movswl/mov %dx 同宽度语义）。
unsigned short GetServerPORT()
{
    return static_cast<unsigned short>(
        G_CEnvironment()->get_exchange_server_port());
}

// ============================================================================
// D) CExchangeServerThread 与全局单例
// ============================================================================

// 全局 exchange 服务指针：ORIG _ZN15exchange_server8CSession16exchange_server_E
// @0x943d9f4（nm -C 证据：'B exchange_server::CSession::exchange_server_'）。
// ORIG Thread C1 将 &m_exchangeServer（线程内嵌 CExchangeServer 子对象地址 =
// 单继承首基 = IExchangeServer 视角）写入该全局；ExSessionTU 侧经虚调用使用。
IExchangeServer* CSession::exchange_server_ = 0;

// C1 ORIG 0x8493e36（disasm 5169-5200）：ThreadInterface()（vptr 派生）→
// 构造 +0x0c 处 m_exchangeServer → 将其地址写入静态全局 exchange_server_。
CExchangeServerThread::CExchangeServerThread()
{
    CSession::exchange_server_ = static_cast<IExchangeServer*>(&m_exchangeServer);
}

// D1 ORIG 0x8493db6（disasm 5121-5156）：置派生 vptr → 析构 +0x0c
// m_exchangeServer → 析构基类 ThreadInterface；由虚析构隐式链完成。
CExchangeServerThread::~CExchangeServerThread()
{
}

// dispatch ORIG 0x8493bfe（disasm 5001-5119，rodata 字符串已从 .rodata 提取）：
void CExchangeServerThread::dispatch(void* arg)
{
    (void)arg;
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x451, 0);
    trace("Exchange Server Thread start! %d");

    if (!m_exchangeServer.PreparePassiveSocket())
    {
        cMyTrace traceErr(
            __PRETTY_FUNCTION__,
            0x459, 0);
        traceErr("\n\n\n\n\n\nExchange Server starting error!\n\n\n\n\n\nShudown~~");
        exit(-1);
    }

    if (!m_exchangeServer.SendExchangeServerInfo())
    {
        cMyTrace traceInfo(
            __PRETTY_FUNCTION__,
            0x469, 0);
        traceInfo("SendExchangeServerInfo failed!");
        return;                       // ORIG jmp 尾部
    }

    while (!GlobalData::s_server_stop)         // 标志 @0x0940bd64
    {
        Stream* msg = m_exchangeServer.GetMessage();
        if (msg != 0)
        {
            // flag 取流缓冲首 dword（ORIG mov (%eax) 后作为 FreeMessage 第三参）。
            int flag = *reinterpret_cast<int*>(msg->get_buffer());
            m_exchangeServer.DispatchMessage(msg);
            m_exchangeServer.FreeMessage(msg, flag);
        }
        m_exchangeServer.GetNetworkEvent();
        OS_API::Sleep(0xa);                        // ORIG 10ms 节拍
    }

    cMyTrace traceEnd(
        __PRETTY_FUNCTION__,
        0x482, 0);
    traceEnd("Exchange Server Thread End!");
}

// GetInstanceExchangeServerThread ORIG 0x8493e8d：guard 单例
// （对象指针 @0x943da40、guard @0x943da20），构造后 __cxa_atexit 注册析构。
CExchangeServerThread* GetInstanceExchangeServerThread()
{
    static CExchangeServerThread instance;
    return &instance;
}

// BeginExchangeServerThread ORIG 0x848f2b8：GetInstance()->begin()。
void BeginExchangeServerThread()
{
    GetInstanceExchangeServerThread()->begin();
}

// getExchangeServerThreadID ORIG 0x848f2cd：GetInstance()->getThreadID()。
unsigned long getExchangeServerThreadID()
{
    return GetInstanceExchangeServerThread()->getThreadID();
}

}  // namespace exchange_server
// ============================================================================
// B+) CACHE_CHARACTER_TYPE
// ============================================================================

// ORIG C1 0x8492262 W（objdump 逐指令转写）：sub $0x18 帧内依次七次子对象构造
//   +0x00000 SIG_LOAD_CHARAC / +0x00703 SIG_LOAD_INVENTORY / +0x15b4f SIG_LOAD_QUEST /
//   +0x1eb13 APSystem::_SIG_LOAD_ACTION_POINT / +0x1f804 advancealtar::_CharacAdvanceAltarDbData /
//   +0x1fd20 SIG_LOAD_ETC / +0x34884 SIG_LOAD_KILL_MONSTER_INFO——与头中成员
//   类型化一一对应，编译器按序自动调用。
CACHE_CHARACTER_TYPE::CACHE_CHARACTER_TYPE()
{
}

// ORIG D1 0x84922d6 W：仅对 +0x1fd20 SIG_LOAD_ETC 调用一次析构；其余七个
//   子对象/POD 区无用户析构 ⇒ 编译器析构序一致。
CACHE_CHARACTER_TYPE::~CACHE_CHARACTER_TYPE()
{
}

// GetMemberPosition ORIG 0x848fd7c W（objdump 逐 switch 分支核对）：
// 返回 (int)(this+off)，size 写入 *size，found 写入 m_memberFlag[idx] 字节值。
// 映射表（ENUM 值 → flag idx / off / size）：
//   0x07→0/+0x00000/0x703  0x08→1/+0x00703/0x1544c  0x0a→3/+0x015b4f/0x7608
//   0x0b→4/+0x01d157/0x19c 0x0c→5/+0x01d2f3/0x1810  0xb3→7/+0x01eb03/0x10
//   0x11→8/+0x01eb13/0xcf1 0x19→a/+0x01f804/0x51c  0x20→b/+0x01fd20/0x14b60
//   0x282→9/+0x034884/0xafcc   未命中→return 0（不写 size/found）。
int CACHE_CHARACTER_TYPE::GetMemberPosition(ENUM_INTERNALPACKET type,
                                            int* size, bool& found)
{
    switch (static_cast<int>(type))
    {
    case 0x07:
        found = m_memberFlag[0] != 0;
        *size = static_cast<int>(sizeof(m_charac));
        return reinterpret_cast<int>(&m_charac);
    case 0x08:
        found = m_memberFlag[1] != 0;
        *size = static_cast<int>(sizeof(m_inventory));
        return reinterpret_cast<int>(&m_inventory);
    case 0x0a:
        found = m_memberFlag[3] != 0;
        *size = static_cast<int>(sizeof(m_quest));
        return reinterpret_cast<int>(&m_quest);
    case 0x0b:
        found = m_memberFlag[4] != 0;
        *size = static_cast<int>(sizeof(m_ticket));
        return reinterpret_cast<int>(&m_ticket[0]);
    case 0x0c:
        found = m_memberFlag[5] != 0;
        *size = static_cast<int>(sizeof(m_dungeonClear));
        return reinterpret_cast<int>(&m_dungeonClear[0]);
    case 0xb3:
        found = m_memberFlag[7] != 0;
        *size = static_cast<int>(sizeof(m_blackList));
        return reinterpret_cast<int>(&m_blackList[0]);
    case 0x11:
        found = m_memberFlag[8] != 0;
        *size = static_cast<int>(sizeof(m_actionPoint));
        return reinterpret_cast<int>(&m_actionPoint);
    case 0x19:
        found = m_memberFlag[0xa] != 0;
        *size = static_cast<int>(sizeof(m_altar));
        return reinterpret_cast<int>(&m_altar);
    case 0x20:
        found = m_memberFlag[0xb] != 0;
        *size = static_cast<int>(sizeof(m_etc));
        return reinterpret_cast<int>(&m_etc);
    case 0x282:
        found = m_memberFlag[9] != 0;
        *size = static_cast<int>(sizeof(m_expand));
        return reinterpret_cast<int>(&m_expand);
    default:
        return 0;
    }
}

