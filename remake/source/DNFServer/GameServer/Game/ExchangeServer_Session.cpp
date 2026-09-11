// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExchangeServer_Session/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ExchangeServer.h"
#include "CNetwork.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GlobalData.h"           // GlobalData::s_systemTime_（get_ms_tick）
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
// ---- get_ms_tick（ORIG _Z11get_ms_tickv @0x086b1b2f；报告
//      docs/class_func_reports/_lt_global_gt_/get_ms_tick.md。唯一 owner=本 TU，
//      已与宿主 OnlyOrig_Exchange 协调登记。）----
long long get_ms_tick()
{
    return (long long)GlobalData::s_systemTime_.getCurTickCount();
}

namespace exchange_server
{

// [WorldLane 2026-09-11 · static 多副本普查] 自共享头 ExchangeServer.h 下沉：
// 唯一使用点在本 TU（145/166 两处）；ORIG `_ZN15exchange_serverL11HEADER_SIZEE`
// = 单份 local r 4B @0x08c79790（仅在 1 TU 发射）⇒ namespace-scope const 放头里
// 会随 include 链被 71 个 TU 各发射一份。此处单份定义与 ORIG 1:1。
const unsigned int HEADER_SIZE = 0x0d;

// ORIG：虚调用点全部内联装载全局（mov 0x943d9f4,%eax），无 helper call。
// 用宏保持 -O0 内联形（P49：static 函数 -O0 生真实 call，与 ORIG 不符）。
#define ES() ((IExchangeServer*)CSession::exchange_server_)

// ---------------------------------------------------------------------------
// ctor — ORIG 0x848ffd0 size 0x53：
// 基类构造后清零 +0x851a0/+0x851a4、+0xc 置 1。
// [b28-L11 翻案 b22判词·N] ORIG 基类 ctor 后于栈帧建匿名 CNetwork<95000,450000>
// dead 临时（lea -0x851a8;C1;D1）撑 sub $0x851b8 巨型帧。dead temp 直书 +
// ~CNetwork()/CNetwork() throw()（P216）后 4.4 零 EH 代价、帧精确复现；
// +0xc 单字节写用 P82 具名视图折叠单条 disp（旧取址形展 add+movb 两条）。
// ---------------------------------------------------------------------------
// P82 具名尾部视图：ORIG `movb $0x1,0xc(%eax)` 单条 disp 写（int 字段低字节）。
// `*(char*)&m_field0c` 取址形 4.4 展成 add+movb 两条；视图成员访问折叠单条。
namespace {
struct NetFlagView
{
    char m_pad00[0xc];   // +0x00..+0x0b
    char m_flag0c;       // +0x0c（CNetwork::m_field0c 低字节，ORIG movb 实证）
};
}  // namespace

#include "ExchangeServer_Session/CSession.inc"
{
    // [b28-L11 复验] 先按 ORIG 直书 dead temp 实测 4.4 EH 代价（b22 判词称 +15）。
    {
        CNetwork<95000, 450000> deadTmp;
    }
    m_triggerEventType = 0;
    m_channelNo = 0;
    // ORIG movb $0x1,0xc(%eax)（仅写低字节；throw() 后 dead-temp 直书零 EH 代价）
    ((NetFlagView*)this)->m_flag0c = 1;
}

// D1 — ORIG 0x8495662 size 0x13，仅调基类析构（隐式空体一致）。
#include "ExchangeServer_Session/dtor_CSession.inc"
{
}

#include "ExchangeServer_Session/GetSocket.inc"
{
    return get_socket();
}

#include "ExchangeServer_Session/SetChannelNo.inc"
{
    // int 字段 + int 形参同型直赋（无转换节点）：4.4 LHS 先载=ORIG 实证。
    m_channelNo = channelNo;
}

#include "ExchangeServer_Session/GetChannelNo.inc"
{
    return (int)m_channelNo;
}

#include "ExchangeServer_Session/SetTriggerSessionEventType.inc"
{
    // int 字段 + int 形参同型直赋（无转换节点）：4.4 LHS 先载=ORIG 实证。
    m_triggerEventType = type;
}

#include "ExchangeServer_Session/AddTriggerSessionEventType.inc"
{
    // int 字段 |= int 形参（无转换节点）：4.4 与 ORIG 逐条同形（原 N18a 判词
    // 在 unsigned 字段+cast 异型下成立；同型化后翻 identical）。
    m_triggerEventType |= type;
}

#include "ExchangeServer_Session/GetTriggerSessionEventType.inc"
{
    return (int)m_triggerEventType;
}

// ---------------------------------------------------------------------------
// Parsing — ORIG 0x8490074 size 0x29a
// ---------------------------------------------------------------------------
// [b28d-68 修法] ORIG 0x08490074。
// ①控制流按 ORIG 重写为 do{...}while(parsingLength>5)（ORIG 0x084900b5 packetSize=0
//   后 jmp 循环体、0x0849020c `jg` 回跳）；memcpy 无条件执行（ORIG 0x084900df 直落，
//   非 else 分支）——删我方发明的 `m_recvOffset==0 && parsingLength<=5 → return 1`
//   与 `m_recvOffset==0 → packetSize=0` 两条分支（ORIG 无）；
// ②DispatchPacket 返回值存 int moved，`if (moved != 0)` 才 trace（ORIG 0x08490195
//   `mov %eax,-0xc` / `cmpl $0x0` / `je`），替换我方 bool dispatched 取反；
// ③logFormat 第三参改 __PRETTY_FUNCTION__（ORIG 0x8c79d00 串实证 =
//   "bool exchange_server::CSession::Parsing(int)"），0x18d 段补 recvLen 实参
//   （ORIG 7 参：0x14(%esp)=recvLen、0x18(%esp)=packetSize；原 "DispatchPacket()"
//   第三参为发明字符串）。
// 残差 N104/EH：4.4 对 try 内仅 nothrow memmove 的 EH 边整体消除（探针
// /tmp/d68probe/eh.o、eh2.o 三形实证：无 __cxa_begin_catch/_Unwind_Resume），
// 4.1 保留 ⇒ p167 的 5 项 EH MISSING 属跨版本结构性不可清。
// [M1S5 2026-09-11 本波两项，ndiff 195→188] ①**类型轴**：ORIG 0x08490090/0x08490093
// `mov %eax,-0x20(%ebp); mov %edx,-0x1c(%ebp)` 后 0x08490099..0x084900a2 两次
// **直传**（`mov %eax,0x4(%ecx); mov %edx,0x8(%ecx)`，无移位指令）⇒ `tick` 为
// **无符号**（有符号 `tick>>32` 必产 `sar $0x1f,%edx`，实证我方旧形含该条）
// ⇒ 改 `unsigned long long tick`；②P339 声明序：ORIG 槽 深→浅 =
// parsingLength(-0x24)/tick(-0x20,-0x1c)/packetSize(-0x14)/moved(-0xc)
// ⇒ 把 tick 提到 packetSize 之前（我方旧序 parsingLength/packetSize/moved/tick
// 使 tick 落最浅 -0x10/-0xc）。our 161→160。
// 残余（判 N，逐条 ORIG 证据）：③`lea (%edx,%eax,1)`(0x08490085) vs 4.4 `add`
//   满负荷位移形（P75 跨版本）；④0x08490219 `jle 尾块`(共享 `mov $1,%ebx` 出口
//   0x08490300) vs 4.4 `jg + mov $1,%eax; jmp`（P325 反向）；⑤do-while 尾
//   0x0849020c `jg` vs 4.4 `setg %al; test %al,%al; jne`（条件物化差）；
//   ⑥`(unsigned int)(tick>>32)` 4.4 产 `mov %edx,%eax; xor %edx,%edx; mov %eax,%edx`
//   三连规范形（4.1 无）；⑦N104 EH 段（上述）。
#include "ExchangeServer_Session/Parsing.inc"
{
    int parsingLength;
    unsigned long long tick;
    unsigned int packetSize;
    int moved;
    char header[HEADER_SIZE];

    parsingLength = (int)m_recvOffset + recvLen;

    tick = get_ms_tick();
    m_lastRecvTick = (unsigned int)tick;
    m_recvGapTick = (unsigned int)(tick >> 32);

    if (parsingLength <= 12)
    {
        return 1;
    }

    packetSize = 0;
    do
    {
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
        }

        memcpy(header, (const void*)m_recvBuf, HEADER_SIZE);
        packetSize = *(unsigned int*)(header + 3);
        if (packetSize <= 0xc || packetSize > 0x17318)
        {
            LogManager::logFormat(1, "ExchangeServerNetwork.h",
                __PRETTY_FUNCTION__, 0x18d,
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large",
                recvLen, (int)packetSize);
            return 0;
        }

        if (parsingLength < (int)packetSize)
        {
            break;
        }

        if (ES() != 0)
        {
            moved = ES()->DispatchPacket(this, (char*)m_recvBuf, (int)packetSize);
            if (moved != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x197, 0)(
                    "DispatchPacket() error line number(%d)", 407);
            }
        }

        parsingLength -= (int)packetSize;
        m_recvBuf += (int)packetSize;
        m_recvOffset = 0;

        if (parsingLength == 0)
        {
            m_recvBuf = m_recv;
            break;
        }
    } while (parsingLength > 5);

    if (parsingLength <= 0)
    {
        return 1;
    }
    if (parsingLength > 0x17318)
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            __PRETTY_FUNCTION__, 0x1b7,
            "[PARSING LENGTH EXCEPTION] parsinglength > MAX_SESSION_BUFFER_SIZE , memmove : parsinglength = %d",
            parsingLength);
        return 0;
    }

    try
    {
        p8_memmove_eh(m_recv, m_recvBuf, (unsigned int)parsingLength);
        m_recvOffset = parsingLength;
        m_recvBuf = m_recv + parsingLength;
    }
    catch (...)
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            __PRETTY_FUNCTION__, 0x1c1,
            "[PARSING EXCEPTION] memmove : parsinglength = %d", parsingLength);
        return 0;
    }

    return 1;
}

// ---------------------------------------------------------------------------
// OnRecv — ORIG 0x849030e size 0x55
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnRecv.inc"
{
    int ret = recv_packet();
    if (ret > 0)
    {
        // b28d-68 P0 清绑：删 register asm("eax") 绑定，改 bool callee 自然形
        // （P292：`if (!Parsing(ret))` 直产 `call; xor $0x1,%eax; test %al,%al; je`，
        // 与 ORIG 0x0849033a..0x0849034d 逐条同形；X1 调用点自救已废）。
        if (!Parsing(ret))
            return false;                     // 失败分支（Parsing 返回 0）
        return true;                          // 成功（Parsing 返回 1）
    }
    if (ret < 0)
    {
        return false;                          // eax=0 错误路径
    }
    return true;                              // ret==0 → eax=1
}

// ---------------------------------------------------------------------------
// OnSend — ORIG 0x8490364 size 0x8a
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnSend.inc"
{
    // ORIG 0x8490364：call send_packet 后 shr $0x1f,%eax 直测符号位，无局部槽
    // （P0 槽-局部律：寄存器直传 ⇒ 内联右值）。
    if (CNetwork<95000, 450000>::send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e4, 0)("OnSend() Socket[%d] Error(%s)",
            GetSocket(), strerror(errno));
        return -1;
    }
    // ORIG 尾部读 this+0x18(m_recvOffset) 作为返回值（unsigned int → int）。
    return m_recvOffset;
}
// OnError — ORIG 0x84903ee size 0x70
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnError.inc"
{
    cMyTrace(__PRETTY_FUNCTION__, 0x1ec, 0)("ExchangeServer OnError() Socket[%d] Error(%s)",
        GetSocket(), strerror(errno));
    return false;
}

// ---------------------------------------------------------------------------
// OnAccept — ORIG 0x849045e size 0x174
// ---------------------------------------------------------------------------
// [b28d-D33 N7/N109 封顶] ORIG 0x0849045e。p167 CLEAR 13=13。
// [M1S5 2026-09-11 **推翻 D33 旧判词**] 两处新钥匙使本件 DIFF→AE：
// ①P0/P329：删 `port`/`ip` 两个具名局部（ORIG -0x10/-0x12 无此二槽、值走
//   callee-saved %esi/%ebx）⇒ 内联进 cMyTrace 实参，帧 0x84→0x70=ORIG、
//   addr/addrlen/cMyTrace 槽全部归位（-0x50/-0x54/-0x40/-0x30/-0x20）；
// ②P307：`register socklen_t* plen = &addrlen;` 使取址实参提前物化 —— 复现
//   ORIG 0x0849046d `lea -0x54(%ebp),%esi`（函数头、GetSocket 前）+ 调用点
//   `mov %esi,0x8(%esp)`（4.4 裸形恒延迟到调用点用 %edx 中转）。
// 读数：our_insns 94=94（持平）；TU 四数 ae 13→14、合计 16→17、near 0→1。
// 残余：无（唯一差异为 .o 未重定位的 call/数据地址显示形）。
#include "ExchangeServer_Session/OnAccept.inc"
{
    sockaddr_in addr;                         // -0x50(%ebp)，len@-0x54
    socklen_t addrlen = 0x10;
    register socklen_t* plen = &addrlen;      // P307：取址实参提前物化

    int fd = accept(GetSocket(), (sockaddr*)&addr, plen);
    if (fd < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f7, 0)("Accept Socket[%d] Error(%s)",
            fd, strerror(errno));
        return 0;                             // mov $0,%eax
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x1fa, 0)(
        "Accepted other game server ip(%s), port(%d), sock(%d)",
        inet_ntoa(addr.sin_addr), (int)ntohs(addr.sin_port), fd);

    // vtable+8 AllocSession()
    CSession* session = (CSession*)ES()->AllocSession();
    if (session != 0)
    {
        session->set_socket(fd, false);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x202, 0)("session_pool_.Acquire() error");
    }
    return session;                           // ORIG 尾：mov -0xc,%eax
}

// ---------------------------------------------------------------------------
// OnConnect — ORIG 0x84905d2 size 0xfe
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/OnConnect.inc"
{
    // vtable+0x10（slot5）GetServerType()；sete 比较返回值==1(EXCHANGE_SERVER_SLAVE)。
    // [b28c-6][N7] ES() 虚调用槽寄存器 %eax↔%edx 互换（4.4 恒形），源码不可翻（ndiff 8）。
    if ((int)ES()->GetServerType() == 1)
    {
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0x82);
        guard.put_int(GetServerChannelNo());
        guard.put_int((int)inet_addr(GetServerIP()));
        guard.put_short(GetServerPORT());
        guard.finalize(false);          // ORIG 0x8490681：Send 前 finalize，否则 Send 判定未封包不发
        // ORIG 0x8490698：mov %eax,%ebx 跨 dtor 存活后 mov %ebx,%eax 透传（bool→bool，
        // 无 test/setne 规范化——Send 真实返回 bool，PRETTY_FUNCTION 串实证）。
        return Send(guard);
    }
    return true;                        // ORIG 0x84906c2：mov $1,%ebx
}

// ---------------------------------------------------------------------------
// Send — ORIG 0x84906d0 size 0x97
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Send.inc"
{
    // ORIG 0x84906d0：test %al,%al + je（正向 if，logFormat 块为 je 目标后置，
    // P32：!f() 会生 xor $1 物化而 ORIG 无）。send_packet 失败路径独立 mov$0;jmp
    // 出口不经 logFormat。
    if (guard.is_finallized())
    {
        // ORIG 0x84906f1：get_len 结果升格 %ebx 跨 get_packet 调用存活（P52），
        // get_packet 结果进 eax 直接入参槽不落栈槽——单语句右到左实参求值形。
        int ret = send_packet(guard.get_packet(0), guard.get_len());
        if (ret < 0)
        {
            return 0;
        }
    }
    else
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            // [M1Free76to90 2026-09-11 · P139] 原手写串缺返回类型/类限定；ORIG 局部静态
            // `_ZZN15exchange_server8CSession4SendER11PacketGuardE19__PRETTY_FUNCTION__`
            // @0x08c79b40 size **0x33** = `bool exchange_server::CSession::Send(PacketGuard&)`
            // （声明已是 bool ⇒ 展开逐字节一致）。
            __PRETTY_FUNCTION__, 0x228,
            "Packet.is_finallized return false");
        return 0;
    }
    return 1;                                 // ORIG L1: mov $1（块在 logFormat 后）
}

// ---------------------------------------------------------------------------
// Listen — ORIG 0x8490767 size 0xc2
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Listen.inc"
{
    int sock;                                 // -0x20(%ebp)
    // [b28c-6][N7] AllocSession 虚调用槽寄存器 %eax↔%edx 互换（4.4 恒形），源码不可翻（ndiff 8）。
    // ORIG：静态调用 CreateListenSocket(&sock, port, ip)（无对象/无 this）；
    // 直接 !返回值 判断（ORIG xor $0x1,%eax; test %al,%al 形态）。
    if (!CNetwork<95000, 450000>::CreateListenSocket(sock, port, ip))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x236, 0)(
            "CreateListenSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
        return session;                       // ORIG mov -0xc,%eax + jmp 岛
    }
    return 0;                                  // ORIG 0x849081e: mov $0,%eax（je 目标后置块）
}

// ---------------------------------------------------------------------------
// Connect — ORIG 0x8490829 size 0xc9
// [N96 清绑 2026-09-09] 删除历史 `register bool failed asm("eax")`（P0 限定② /
// N96 全域禁用；P248 不得恢复）。报告缺失（docs/class_func_reports 无
// exchange_server 目录；同名 CDBConnector/Connect.md 经签名核对为撞名错配，
// 仅用 ORIG 二进制切片，§3.2 兜底并标注）。ORIG 0x849084c
// `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je`——failed 值全程在寄存器、
// 无栈槽 ⇒ 原码为裸 register 局部（当时的 `if (ok != 0)` 直判是过度删除）。
// [M1S5 2026-09-11 **修正 N96 清绑过度删除**] 裸 `register`（**无 asm 绑定**）是
// 合法存储类提示（P231；docs/asm_translation_patterns.md 该条登记本文件即此形），
// N96/P0 禁的是 `register T x asm("reg")` 显式绑定。恢复
// `register bool failed = (ok != 0); if (failed)` 后复现 ORIG 0x849084c 形
// `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je`（4.4 分配 %bl 而非 4.1 的
// %al，寄存器角色差、助记符序列全等）。
// 读数：清绑过度 [ae=13 合计16 near=0 diff=4，orig57/our55，缺 setne+test 两条]
//       → 本波 [near=1 diff=3，orig57/our57]；本函数 DIFF→NEAR。
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Connect.inc"
{
    int sock;                                 // -0x24(%ebp)
    // [b28c-6][N7] AllocSession 虚调用槽寄存器 %eax↔%edx 互换（4.4 恒形），源码不可翻（ndiff 8）。
    // CreateConnectionSocket 为静态成员（ORIG 同）；返回非 0=失败码。
    const int ok = CNetwork<95000, 450000>::CreateConnectionSocket(sock, ip, port);

    // [N96 清绑] 原 `register bool failed asm("eax")` 已删（P0/N96 全域禁绑）。
    // ORIG 0x849084c `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je` ——
    // failed 值全程在寄存器、无栈槽 ⇒ 原码为**裸 register 存储类提示**
    // （P231：禁的是 `register T x asm("reg")` 绑定，裸 register 合法且 -O0
    // 仍参与分配决策；本形即 docs 中登记的 ExchangeServer_Session 计数族）。
    // [C4A0Nine 2026-09-11 复验] 另 4 形全败：`register char failed` /
    //   `register const bool` / 分离赋值（先声明后赋）/ `= !!ok` ⇒ 4.4.7 恒
    //   `setne %bl; test %bl,%bl`（57/58 不变）。ORIG 0x08490850 `setne %al` 为
    //   4.1 QImode 分配独有形 ⇒ N7/N109 封顶。
    register bool failed = (ok != 0);
    if (failed)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24b, 0)(
            "CreateConnectionSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
        return session;
    }
    return 0;                                  // ORIG 0x84908e7: mov $0,%eax
}

// ---------------------------------------------------------------------------
// Disconnect — ORIG 0x84908f2 size 0x9f（P159：函数尾 `mov $0x1,%eax` ⇒ bool true；
// b28-L11 补 return true：gap 1→0；残差 12 全为 vtable+12 FreeSession 虚调用
// N7 寄存器互换（{ecx,eax,edx} 组，4.4 间接 call 恒 %eax，负例表 N7 附）。
// ---------------------------------------------------------------------------
#include "ExchangeServer_Session/Disconnect.inc"
{
    // [b28c-6] N7：ES() 虚调用槽寄存器 %ecx↔%eax 互换 + this 中转寄存器互换
    // （ORIG `mov (%eax),%ecx; mov <A>,%eax; mov this,%edx; mov %edx,0x4(%esp);
    // mov %eax,(%esp); call *%ecx`），4.4 恒目标 %eax，源码不可翻（ndiff 12）。
    cMyTrace(__PRETTY_FUNCTION__, 0x25e, 0)("Disconnect Exchange Server");

    if (CSession::exchange_server_ != 0)
    {
        // vtable+4 UnregisterExchangeServer(channelNo)
        ES()->UnregisterExchangeServer(GetChannelNo());
    }

    disconnect();

    // vtable+12 FreeSession(this)
    ES()->FreeSession(this);
    return true;   // ORIG 尾 `mov $0x1,%eax`
}

}  // namespace exchange_server
