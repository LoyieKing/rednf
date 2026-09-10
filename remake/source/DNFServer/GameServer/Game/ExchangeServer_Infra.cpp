// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExchangeServer_Infra/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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

// ORIG C1 0x8490e98：Mutex@+0x00 / StaticPool@+0x18 / std::map@+0x48
// 按声明序隐式构造。
#include "ExchangeServer_Infra/CCacheCharacterMgr.inc"
}

// ORIG D1 0x8490f04：子对象按逆序隐式析构。
#include "ExchangeServer_Infra/dtor_CCacheCharacterMgr.inc"
}

// AcquireCharacter ORIG 0x8490a8a：map 命中→memset+刷新 cacheTime 直接复用；
// 未命中且池空→CollectGarbage 回收，仍无则失败返回 0；否则池 Acquire+清零。
// [b28d-solo-ExchangeServer_Infra 判词·AE] 三路（命中直返 / 池空回收失败返 0 /
// 池 Acquire 清零）与 ORIG 逐条全等；池 Acquire 后 `while (mem != 0) {…; break;}`
// （P225）复现 ORIG 四段物化 `cmpl $0,-0xc; setne %al; test %al,%al; je`。
#include "ExchangeServer_Infra/AcquireCharacter.inc"
}

// FreeCharacter ORIG 0x8490bf4：池 Free；(1000-remain)!=map.size() 时 cMyTrace 告警
// （line=0x4e）。
// [b28d-lane15] 已修：Size() 常量 1000（P119 头内 inline 4.4 恒发 call）、trace
// 全表达式临时链（P37/P227）、格式串按 ORIG .rodata 0x08c78398 实测
// "FreeCharacter() error (%d,%d,%d)" 且实参序 (1000,remain,size)。
// [b28d-solo-ExchangeServer_Infra 判词·AE] 原 N 判词（N7/N18a 四段中转不可造）作废：
// ORIG 0x8490c31 链 `mov $0x3e8,%edx; mov %edx,%ecx; sub %eax,%ecx; mov %ecx,%eax;
// mov %eax,%ebx` 的 ecx→eax 中转来自 **unsigned 域**（P248 档③ cast 旋钮）——条件写
// `static_cast<unsigned int>(1000 - m_pool.Remain()) != m_map.size()` 即逐条全等。
#include "ExchangeServer_Infra/FreeCharacter.inc"
}

// CacheCharacter ORIG 0x8490dca：insert，已存在则更新指针，恒 true。
#include "ExchangeServer_Infra/CacheCharacter.inc"
}

// GetCacheCharacter ORIG 0x8490cee：查 map，erase 可选。
#include "ExchangeServer_Infra/GetCacheCharacter.inc"
}

// CollectGarbage ORIG 0x8490992：回收 cacheTime 距今 >0x1d 秒的条目。
// [b28d-solo-ExchangeServer_Infra 判词·AE] 原 N 判词（N82/N93 槽布局）作废：
// ORIG 0x8490a3c 增量段调 `_ZSt17_Rb_tree_iterator…ppEi`（后置 ++，带 sret 临时槽
// -0x14）并把旧值直传 erase ⇒ 原码是 `m_map.erase(it++);`（P28 增量侧推论）；
// 具名 cur 拷贝 + 前置 ++it 会改发 ppEv 并丢临时槽（it 槽 -0x1c→-0x18）。改后全等。
#include "ExchangeServer_Infra/CollectGarbage.inc"
}

// GetInstanceCacheCharacterMgr ORIG 0x8490f71：guard 单例（对象 @0x940b55c、
// guard @0x9405220）——由函数级静态局部产生等价 __cxa_guard ABI。
CCacheCharacterMgr* GetInstanceCacheCharacterMgr()
{
    static CCacheCharacterMgr instance;
    return &instance;
}

#include "ExchangeServer_Infra/FreeCharacterMemory.inc"
}

#include "ExchangeServer_Infra/CacheCharacterMemory.inc"
}

// ============================================================================
// C) CExchangeServerHitRate —— 六字段布局/回绕语义见 ExchangeServer.h 私区注释
// ============================================================================

// ORIG C1 0x82346fc：仅调 Reset()。
#include "ExchangeServer_Infra/CExchangeServerHitRate.inc"
}

// Reset ORIG 0x82346bc：六槽清零。
#include "ExchangeServer_Infra/Reset.inc"
}

// IncrementDBLoadCount ORIG 0x823465a：+0 自增，归零(回绕)则 Overflow(+8) +1。
#include "ExchangeServer_Infra/IncrementDBLoadCount.inc"
}

// IncrementCacheLoadCount ORIG 0x823468a：+4 自增，归零则 (+0xc) +1。
#include "ExchangeServer_Infra/IncrementCacheLoadCount.inc"
}

// IncrementCacheLoadFailCount ORIG 0x848f35e：+0x10 自增，归零则 (+0x14) +1。
#include "ExchangeServer_Infra/IncrementCacheLoadFailCount.inc"
}

#include "ExchangeServer_Infra/GetDBLoadCount.inc"
}

#include "ExchangeServer_Infra/GetCacheLoadCount.inc"
}

#include "ExchangeServer_Infra/GetDBLoadOverflowCount.inc"
}

#include "ExchangeServer_Infra/GetCacheLoadOverflowCount.inc"
}

#include "ExchangeServer_Infra/GetCacheLoadFailCount.inc"
}

#include "ExchangeServer_Infra/GetCacheLoadFailOverflowCount.inc"
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
#include "ExchangeServer_Infra/GetServerChannelNo.inc"
}

// GetServerIP ORIG 0x848f28e：G_CEnvironment()->get_exchange_server_ip()。
char* GetServerIP()
{
    return G_CEnvironment()->get_exchange_server_ip();
}

// GetServerPORT ORIG 0x848f2a3：G_CEnvironment()->get_exchange_server_port()
// （int 截 ushort 返回，ORIG movswl/mov %dx 同宽度语义）。
#include "ExchangeServer_Infra/GetServerPORT.inc"
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
#include "ExchangeServer_Infra/CExchangeServerThread.inc"
}

// D1 ORIG 0x8493db6（disasm 5121-5156）：置派生 vptr → 析构 +0x0c
// m_exchangeServer → 析构基类 ThreadInterface；由虚析构隐式链完成。
#include "ExchangeServer_Infra/dtor_CExchangeServerThread.inc"
}

// dispatch ORIG 0x8493bfe（disasm 5001-5119，rodata 字符串已从 .rodata 提取）：
// [b28d-solo-ExchangeServer_Infra 判词·AE] 原 N 判词（N93/N82 cMyTrace 槽组）作废：
// ①四个 cMyTrace 改「构造即调用」全表达式临时链（P221/P227）⇒ 槽
// -0x50/-0x40/-0x30/-0x20 与 ORIG 全等；②msg 局部类型 int→Stream*（GetMessage
// 原返回类型）后 DispatchMessage/FreeMessage 段 this 先装载 + `lea 0xc(%eax),%edx`
// 与 ORIG 逐条全等（int 形 4.4 恒先装实参并折 `add $0xc,%edx`）。102=102 AE。
#include "ExchangeServer_Infra/dispatch.inc"
}

// GetInstanceExchangeServerThread ORIG 0x8493e8d：guard 单例
// （对象指针 @0x943da40、guard @0x943da20），构造后 __cxa_atexit 注册析构。
CExchangeServerThread* GetInstanceExchangeServerThread()
{
    static CExchangeServerThread instance;
    return &instance;
}

// BeginExchangeServerThread ORIG 0x848f2b8：GetInstance()->begin()。
#include "ExchangeServer_Infra/BeginExchangeServerThread.inc"
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
#include "ExchangeServer_Infra/CACHE_CHARACTER_TYPE.inc"
}

// ORIG D1 0x84922d6 W：仅对 +0x1fd20 SIG_LOAD_ETC 调用一次析构；其余七个
//   子对象/POD 区无用户析构 ⇒ 编译器析构序一致。
#include "ExchangeServer_Infra/dtor_CACHE_CHARACTER_TYPE.inc"
}

// GetMemberPosition ORIG 0x848fd7c W（objdump 逐 switch 分支核对）：
// 返回 (int)(this+off)，size 写入 *size，found 写入 m_memberFlag[idx] 字节值。
// 映射表（ENUM 值 → flag idx / off / size）：
//   0x07→0/+0x00000/0x703  0x08→1/+0x00703/0x1544c  0x0a→3/+0x015b4f/0x7608
//   0x0b→4/+0x01d157/0x19c 0x0c→5/+0x01d2f3/0x1810  0xb3→7/+0x01eb03/0x10
//   0x11→8/+0x01eb13/0xcf1 0x19→a/+0x01f804/0x51c  0x20→b/+0x01fd20/0x14b60
//   0x282→9/+0x034884/0xafcc   未命中→return 0（不写 size/found）。
#include "ExchangeServer_Infra/GetMemberPosition.inc"
}

