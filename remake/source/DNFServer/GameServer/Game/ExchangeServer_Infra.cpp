// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExchangeServer_Infra/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "ExchangeServer_Infra/Sleep.inc"
}

namespace exchange_server
{

// ============================================================================
// A) CEpoll<CSession> —— ORIG 地址见 ExchangeServer.h 方法注释
// ============================================================================

// ---- 自由函数转发 ORIG 0x848f220/0x848f23c/0x848f258 ------------------------
CACHE_CHARACTER_TYPE* AcquireCharacterMemory(int characNo)
{
    return GetInstanceCacheCharacterMgr()->AcquireCharacter(characNo);
}

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
bool CEpoll<CSession>::RegisterSession(CSession* session, int type)
{
    session->SetTriggerSessionEventType(type);
    return SetEpoll(static_cast<void*>(session), session->GetSocket(), false) == 0;
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
// [D08 2026-09-10] DIFF(170/169) → **NEAR 170=170**（助记符序列全等；TU 三数
// diff 3→2、near 0→1）。修法 = `volatile bool ok`（P332 补②，探针 /tmp/ok_probe.cpp
// f1/f2 对照）：4.4 对 volatile 局部在每使用点强制 `movzbl <slot>,%eax; test %al,%al`
// （复现 ORIG 0x08495f76 装载+寄存器直测），普通 bool 恒折 `cmpb $0,<slot>`。
// 残余仅 ok 槽号 -0x29 vs ORIG -0x1d（4.4 把 volatile 对象放最深 1 字节区；声明
// 上提到函数首 / 裸声明后赋值两形实测无效，`volatile unsigned char` 与
// `*(volatile bool*)&ok` 两形反劣）⇒ N82 附槽位封顶。旧「五形不可翻」判词作废。
template <>
bool CEpoll<CSession>::WaitForEvent(int timeoutMs)
{
    // [D08 2026-09-10] 本件已按上方函头判词落地：`volatile bool ok`（见下），
    // OnConnect/OnError 两处回读复现 ORIG 装载+直测形，残差仅 ok 槽号 -0x29/-0x1d。
    // [b23S3C] P48 去缓存：errno 两比较点各重取（ORIG 两次 __errno_location
    // call、无 err 槽）；EINTR 分支不写 n=0（ORIG 保持负数走空循环）；events[i]
    // .events 掩码每用点重取（ORIG 无 mask 槽，8 条/用点×5）；无 evOk 中间局部
    // ——各 handler 结果直接覆写 allAlive 槽 -0x1d（ORIG `mov %al,-0x1d`×3，
    // 非 && 汇聚岛）；循环头 `movl $0,-0x14` 死置零 ⇒ 原码有名局部初始化。
    epoll_event* events = GetEpollEvents();
    int epfd = GetEpollFD();
    int n = epoll_wait(epfd, events, 0x258, timeoutMs);
    if (n < 0)
    {
        if (errno != EINTR && errno != 0)
            return false;
    }

    volatile bool ok = true;
    for (int i = 0; i < n; ++i)
    {
        CSession* session = static_cast<CSession*>(events[i].data.ptr);
        int regFlags = 0;                 // ORIG 循环头死置零槽（-0x14）
        (void)regFlags;
        ok = true;                        // ORIG 循环头 `movb $0x1,-0x1d` 每轮重置

        if (events[i].events & 0x1)       // EPOLLIN
        {
            if ((session->GetTriggerSessionEventType() & 0x8) != 0)
            {
                // 监听 socket：OnAccept 返回新建会话指针（池 Acquire），非 fd。
                // P197③双槽置0：ORIG OnAccept 前 `movl $0x0,-0x10` 预清。
                // P15 赋值即条件：ORIG `cmpl;setne;test` 物化仅 4.4 在
                // `(x = expr) != 0` 赋值表达式比较处复现（槽比较写法被简化）。
                CSession* accepted = 0;
                if ((accepted = session->OnAccept()) != 0)
                    RegisterSession(accepted, 0x7);
            }
            else if ((session->GetTriggerSessionEventType() & 0x1) != 0)
            {
                ok = session->OnRecv();
            }
        }

        if (events[i].events & 0x4)       // EPOLLOUT
        {
            int sendRet = 0;              // ORIG OUT 块入口 `movl $0x0,-0xc`
            if ((session->GetTriggerSessionEventType() & 0x10) != 0)
            {
                ok = session->OnConnect();
                // [b28d-solo-ExchangeServer_Infra 判词·N（P168a③b 跨版本 bool ABI）]
                // ORIG 0x8496089 回读形 `movzbl -0x1d(%ebp),%eax; test %al,%al; je`；
                // 4.4 对 bool 局部条件恒折单条 `cmpb $0x0,-0x1d(%ebp); je`（少 1 条）。
                // 本波实测五形全部同折：(unsigned char)ok / (int)ok / (unsigned int)ok /
                // 局部类型改 uchar（反使 `if(!ok)` 段也折 cmpb）/ `*reinterpret_cast<
                // unsigned char*>(&ok)`（多 lea+movzbl 且 ok 槽 -0x1d→-0x29 帧漂）。
                // 语义等价（bool 域内 test 与 cmp $0 同判），封顶。
                if (ok)
                    session->SetTriggerSessionEventType(0x7);
            }
            else if ((session->GetTriggerSessionEventType() & 0x2) != 0)
            {
                sendRet = session->OnSend();
                if (sendRet < 0)
                    ok = false;
            }
        }

        if (events[i].events & 0x18)      // EPOLLERR|EPOLLHUP
        {
            ok = session->OnError();      // ORIG `mov %al,-0x1d` 覆写 ok 槽
            if (!ok)                      // 语义同 ORIG；残差在 OnConnect 块槽回读形（见彼处判词）
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
    // [b28d-lane15] P0/N96：原 `register unsigned char v asm("eax")` 绑定已清除。
    // CSession::Send 返回 bool（ExchangeServer.h:77 PRETTY 串实证）⇒ P248 档①
    // 裸形 `if (!boolcall())` 直产 ORIG `call; xor $0x1,%eax; test %al,%al; je`
    // （X2 uchar 局部形会被 4.4 栈化：mov %al,-0x9; xorb $1,-0x9; cmpb $0,-0x9）。
    if (!session->Send(guard))
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
bool CSessionManager<CSession, PacketGuard, CEpoll>::TryListen(
    int port, const char* ip)
{
    CSession* session = 0;
    session = CSession::Listen(port, ip);
    // [b28d-solo-ExchangeServer_Infra] P0/N96 清绑：ORIG 0x8495592 段
    // `cmpl $0x0,-0xc(%ebp); sete %al; test %al,%al; je`（无 z 槽）⇒ P225
    // while+break 族（原 `register bool z asm("al")` 已删）。
    while (session == 0)
    {
        return 0;
        break;
    }
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
    // [b28d-solo-ExchangeServer_Infra] P0/N96 清绑：RegisterSession 返回 bool
    // ⇒ P168a 定论裸形 `if (!boolcall())` 直产 ORIG 0x8495548 段
    // `call; xor $0x1,%eax; test %al,%al; je`（原 uchar v 绑定已删）。
    if (!RegisterSession(session, 0x14))
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
template bool CSessionManager<CSession, PacketGuard, CEpoll>
    ::TryListen(int, const char*);
template bool CSessionManager<CSession, PacketGuard, CEpoll>
    ::TryConnect(const char*, int);


// ============================================================================
// B) CCacheCharacterMgr —— 方法体迁移自 /tmp/df_batches/ex_notes/
//    ExchangeServer_tmp_host.cpp（原 CGameManager.cpp 区段，objdump 已逐函数核对）
// ============================================================================

#include "ExchangeServer_Infra/CCacheCharacterMgr.inc"


#include "ExchangeServer_Infra/dtor_CCacheCharacterMgr.inc"


#include "ExchangeServer_Infra/AcquireCharacter.inc"


#include "ExchangeServer_Infra/FreeCharacter.inc"


#include "ExchangeServer_Infra/CacheCharacter.inc"


#include "ExchangeServer_Infra/GetCacheCharacter.inc"


#include "ExchangeServer_Infra/CollectGarbage.inc"


// GetInstanceCacheCharacterMgr ORIG 0x8490f71：guard 单例（对象 @0x940b55c、
// guard @0x9405220）——由函数级静态局部产生等价 __cxa_guard ABI。
CCacheCharacterMgr* GetInstanceCacheCharacterMgr()
{
    static CCacheCharacterMgr instance;
    return &instance;
}

#include "ExchangeServer_Infra/FreeCharacterMemory.inc"


#include "ExchangeServer_Infra/CacheCharacterMemory.inc"


// ============================================================================
// C) CExchangeServerHitRate —— 六字段布局/回绕语义见 ExchangeServer.h 私区注释
// ============================================================================

#include "ExchangeServer_Infra/CExchangeServerHitRate.inc"


#include "ExchangeServer_Infra/Reset.inc"


#include "ExchangeServer_Infra/IncrementDBLoadCount.inc"


#include "ExchangeServer_Infra/IncrementCacheLoadCount.inc"


#include "ExchangeServer_Infra/IncrementCacheLoadFailCount.inc"


#include "ExchangeServer_Infra/GetDBLoadCount.inc"


#include "ExchangeServer_Infra/GetCacheLoadCount.inc"


#include "ExchangeServer_Infra/GetDBLoadOverflowCount.inc"


#include "ExchangeServer_Infra/GetCacheLoadOverflowCount.inc"


#include "ExchangeServer_Infra/GetCacheLoadFailCount.inc"


#include "ExchangeServer_Infra/GetCacheLoadFailOverflowCount.inc"


// GetInstanceExchangeServerHitRate ORIG 0x823470f：guard 单例
// （对象 @0x940b55c、guard @0x9405220）。
CExchangeServerHitRate* GetInstanceExchangeServerHitRate()
{
    static CExchangeServerHitRate instance;
    return &instance;
}

// ---- 环境读取自由函数 --------------------------------------------------------

#include "ExchangeServer_Infra/GetServerChannelNo.inc"


// GetServerIP ORIG 0x848f28e：G_CEnvironment()->get_exchange_server_ip()。
char* GetServerIP()
{
    return G_CEnvironment()->get_exchange_server_ip();
}

#include "ExchangeServer_Infra/GetServerPORT.inc"


// ============================================================================
// D) CExchangeServerThread 与全局单例
// ============================================================================

// 全局 exchange 服务指针：ORIG _ZN15exchange_server8CSession16exchange_server_E
// @0x943d9f4（nm -C 证据：'B exchange_server::CSession::exchange_server_'）。
// ORIG Thread C1 将 &m_exchangeServer（线程内嵌 CExchangeServer 子对象地址 =
// 单继承首基 = IExchangeServer 视角）写入该全局；ExSessionTU 侧经虚调用使用。
IExchangeServer* CSession::exchange_server_ = 0;

#include "ExchangeServer_Infra/CExchangeServerThread.inc"


#include "ExchangeServer_Infra/dtor_CExchangeServerThread.inc"


#include "ExchangeServer_Infra/dispatch.inc"


// GetInstanceExchangeServerThread ORIG 0x8493e8d：guard 单例
// （对象指针 @0x943da40、guard @0x943da20），构造后 __cxa_atexit 注册析构。
CExchangeServerThread* GetInstanceExchangeServerThread()
{
    static CExchangeServerThread instance;
    return &instance;
}

#include "ExchangeServer_Infra/BeginExchangeServerThread.inc"


// getExchangeServerThreadID ORIG 0x848f2cd：GetInstance()->getThreadID()。
unsigned long getExchangeServerThreadID()
{
    return GetInstanceExchangeServerThread()->getThreadID();
}

}  // namespace exchange_server
// ============================================================================
// B+) CACHE_CHARACTER_TYPE
// ============================================================================

// [SemA06 2026-09-12 · P299/P318 CWG434] 原两处 include（C1/D1 空体定义）已停用：
//   删用户声明改隐式合成后不得再有类外定义（否则“定义无声明”编译错，且会重新
//   把该类变成 user-provided）。两个 .inc 文件本体保留（内容已注释）。
//#include "ExchangeServer_Infra/CACHE_CHARACTER_TYPE.inc"
//#include "ExchangeServer_Infra/dtor_CACHE_CHARACTER_TYPE.inc"


#include "ExchangeServer_Infra/GetMemberPosition.inc"


