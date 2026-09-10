// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TaiwanInternalDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— TaiwanInternalDispatcher（台湾内部分发器，ORIG T @0x081601d0 族）。
// 报告：docs/class_func_reports/TaiwanInternalDispatcher/（6 函数齐全）。
// 依赖：init 中 new 的 10 个子类（InterLoadServiceRestrictPolicy 等）为 ORIG W ctor；
// InterSelectCreateDnfEventInfo 的 ctor 由 OrigScriptInfo 批次实现（本 TU 仅 U 引用），
// 其余 9 个子类 ctor/dtor 最小实现（仅基类 ctor + vptr，证据：global_constructors_
// keyed_to_TaiwanInternalDispatcher 报告各 ctor 区 movl code + call 基类 C1 + 设子类 vtable）。
#include "TaiwanInternalDispatcher.h"
// ============================================================================
// b28d-solo-TaiwanInternalDispatcher 判词表（9 件，fresh .o 复验）：
//   InterUpdateP2pHolePunchingState::dispatch_sig 0x08161952 —— AE
//     （去 CStatisticServerProxy* proxy 具名局部改链式 GetServerProxy()->SendPacket；
//      P0/P52，proxy 槽消失后 v/pkt 槽位与 ORIG -0xc/-0x49 精确对齐）
//   InterLoadHeroMissionEventData::dispatch_sig 0x08161c92 —— AE
//     （sig 视图声明提前至判空之前 + Get() 三次链式内联 + IsFirstTimeLogin bool 化，
//      依赖 CUserCharacInfo.h/.cpp 由 Main 代落；p167 9/9 CLEAR）
//   InterLoadServiceRestrictPolicy::dispatch_sig 0x08160796 —— AE
//     （sig 提前 + std::string 临时全表达式传给 beginPolicy；P37/P221 槽序与
//      ORIG -0xc/-0xd/-0x14 对齐）
//   InterLoadGeolocation::dispatch_sig 0x081608a —— AE
//     （sig 提前 + `> 0x2du` 无符号比较复现 jbe + 两处 cMyTrace 全表达式临时链
//      复现 ORIG 实参 %ebx 跨 ctor 保活；P221/P226）
//   InterCheckStayTimeEvent::dispatch_sig 0x08160a42 —— DIFF·N7+N18a 反向封顶
//     （残余仅 1 处虚调用 make_item：ORIG `mov (%eax),%edx … call *%edx` vs 我方
//      `mov (%eax),%eax … call *%eax` = 4.4.6 虚调用目标恒 %eax（P230 的 N7 只覆盖
//      fnptr 间接调用）；另 ORIG `mov &kRewardItem_,%eax; mov %eax,%ebx` 两段 vs 我方
//      单段 `mov &sym,%ebx` = N18a 反向，`(unsigned int)` 实参 cast 实测无效已回退。
//      p167 16/16 CLEAR、gap 0）
//   InterInsertGiveBoxEventReward::dispatch_sig 0x08161a8a —— DIFF·N7+N18a 反向封顶
//     （与 StayTime 同形同因；p167 16/16 CLEAR、gap 0）
//   InterSelectCreateDnfUserInfo::dispatch_sig 0x08161012 —— AE
//     （Msg 视图 + 判空提前 + eventing 具名 bool 删除（ORIG 无槽）+ rewardable char 化
//      + `!msg->m_received` 短路 + stay/IsEventing 内联变参；p167 15/15 CLEAR）
//   InterSelectMobileAuthReward::dispatch_sig 0x0816132a —— DIFF·N111 封顶
//     （ORIG 0x08161384 为无条件 jmp，其后 0x5a..0x263 发邮件主体（vector pair/
//      findString×2/strncpy×2/9-loop/MakeSystemMultiMailPostal/ReqDBSendNewSystemMultiMail
//      /GetRepeatEvent/SendReward + vector EH cleanup）为**无入口死块**，4.4 恒 DCE ⇒
//      合法 C++ 不可造（N111）。可达路径（tail 0x0816158d `movzbl 0x4(%eax),%edx;
//      mov %dl,0x8ec31(%eax)`）已逐条对齐（CUser.h m_field8ec31 bool 化由 Main 代落）。
//      p167 3 个 MISSING 全在该死块内）
//   TaiwanInternalDispatcher::init 0x08160294 —— AE
//     （10 个子类 ctor 链式块 + `p=0` 预清；关键=InterSelectCreateDnfEventInfo.h:40
//      声明加 __attribute__((nothrow)) 消 4.4 的整段 EH landing + push esi/edi，
//      ORIG 0x0816201a 调用点无 landing 为证；p167 40/40 CLEAR）
// ============================================================================

#include <algorithm>

#include "TaiwanInterfaceInterDispatcher.h"
#include "TaiwanInternal.h"      // Taiwan::internal_stream（makeRequest 依赖）

#include "InterLoadServiceRestrictPolicy.h"
#include "InterLoadGeolocation.h"
#include "InterCheckStayTimeEvent.h"
#include "InterSelectCreateDnfEventInfo.h"
#include "InterSelectCreateDnfUserInfo.h"
#include "InterSelectMobileAuthReward.h"
#include "InterSelectPcroomDailyReward.h"
#include "InterUpdateP2pHolePunchingState.h"
#include "InterInsertGiveBoxEventReward.h"
#include "InterLoadHeroMissionEventData.h"
// OnlyOrigBatch4：InterSelectCreateDnfUserInfo::makeRequest/dispatch_sig 依赖。
#include "GlobalData.h"        // s_stream_pool / s_msgq_mgr / s_event_manager
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "CUser.h"
#include "CEventCreateDnf.h"
// OnlyOrigBatch14：InterSelectMobileAuthReward/InterUpdateP2pHolePunchingState
// dispatch_sig 依赖。
#include "CEventCeraShopRewardPoint.h"
#include "CMailBoxHelper.h"
#include "Inven_Item.h"
#include "RDARScriptStringManager.h"
#include "RestrictGeolocation.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "../../../shared/packet/include/Packet_GameServer2Statisctics2DBServer.h"
#include "CGuildServerProxy.h"   // CEventBase::IsEventing / CEventManager::GetRepeatEvent
#include <cstring>               // strncpy / strlen

// ---- TaiwanInternalDispatcher ----

#include "TaiwanInternalDispatcher/TaiwanInternalDispatcher.inc"
{
    m_dispatchers.clear();   // ORIG @0x081601d0：map::map + map::clear
}

// ---- Inter* 子类最小实现（仅基类 ctor + vptr；业务方法不在本批）----

#include "TaiwanInternalDispatcher/InterLoadServiceRestrictPolicy.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)1)  // ORIG @0x08161ecd movl $0x1
{
}
#include "TaiwanInternalDispatcher/dtor_InterLoadServiceRestrictPolicy.inc"
{
}

#include "TaiwanInternalDispatcher/InterLoadGeolocation.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)4)  // ORIG @0x08161f3f movl $0x4
{
}
#include "TaiwanInternalDispatcher/dtor_InterLoadGeolocation.inc"
{
}

#include "TaiwanInternalDispatcher/InterCheckStayTimeEvent.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)5)  // ORIG @0x08161fb1 movl $0x5
{
}
#include "TaiwanInternalDispatcher/dtor_InterCheckStayTimeEvent.inc"
{
}

#include "TaiwanInternalDispatcher/InterSelectCreateDnfUserInfo.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xa)  // ORIG @0x08162095 movl $0xa
{
}
// ORIG W 0x081620b0/0x081620e0：vptr + 基类析构（D0 另加 operator delete）。
#include "TaiwanInternalDispatcher/dtor_InterSelectCreateDnfUserInfo.inc"
{
}
// ---- makeRequest（InterSelectMobileAuthReward）----
// ORIG T 0x081615ae：同 CreateDnfUserInfo 模式，internal_stream(guard, 0xc,
// param_1)；空 → logFormat(0x18b)；否则写 +0x0/+0x4 入 Msg 并 put(1, guard)。
#include "TaiwanInternalDispatcher/makeRequest.inc"
                                              bool param_3)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x186),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xc), param_1);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x18b, "[Taiwan, MobileAuth] m_id:%d rewarded:%d", param_2,
            // [b28c-32] ORIG 0x08161626 cmpb/je/mov$1/mov$0 三目物化（P47）；
            // (unsigned int)param_3 直转会退化为 movzbl 单条。
            param_3 ? 1 : 0);
    }
    else
    {
        msg->m_id = param_2;
        // ORIG 0x08161218 为 4.1 bool→int 分支物化（mov$1/mov$0）；4.4 对 bool
        // 恒 movzbl 直读不可翻（P168a ③b 族），语义等价标 N。
        msg->m_rewarded = param_3 != 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}

// ---- makeRequest ----
// ORIG T 0x08161218：Acquire stream → CStreamGuard(stream,true) →
// internal_stream(guard, 0xa, uid) → guard->GetInBuffer<Msg>()；
// 写 m_id/received 入 MsgQueueMgr::put(1, guard)。
#include "TaiwanInternalDispatcher/makeRequest_1.inc"
                                               bool received)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x14d),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xa), uid);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x152,
                              "[Taiwan, Event] m_id:%d isReceived:%d",
                              memberId, received);
    }
    else
    {
        msg->m_id = memberId;
        msg->m_received = received;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}

// ---- dispatch_sig ----
// ORIG T 0x08161012。第二参 buf 实际承载 Msg*（判空后读 +0x0/+0x4）。
// 流程：buf==0 → 日志 "interMsg is null." 返回 0x12f；
//   event = GetRepeatEvent(0xa0)；
//   if (event==0 || vtable+0x34 槽(event,0) != 1) 视为「事件未进行」：
//       再调槽取字节 + 日志 stayEvent=ok/nullptr eventing:%d，返回 0x139；
//   否则：SetEventCreateDnfReward(buf+4)；发 0x22d 包（rate + rewardable byte），返回 0。
// 注：vtable+0x34 槽为 CEventBase 判活直调（与 InterSelectCreateDnfEventInfo 同口径，
//   见其 dispatch_sig 注释；ORIG 槽序与 CGuildServerProxy.h 注释有出入，用直调保 ABI）。
// [b28d-solo-TaiwanInternalDispatcher] AE（Msg 视图+eventing 去槽+rewardable char+内联变参）
#include "TaiwanInternalDispatcher/dispatch_sig.inc"
{
    Msg* msg = reinterpret_cast<Msg*>(buf);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x12e,
                              "[Taiwan, Event] interMsg is null.");
        return 0x12f;
    }
    CEventCreateDnf* event =
        (CEventCreateDnf*)GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    // §4.5 真实虚调用 CEventBase::IsEventing(CUser*)@vtable+0x34（CGuildServerProxy.h
    // 权威）。与 ORIG `call *%edx` 相比 GCC 4.4 用 `call *%eax`（寄存器分配 N7 类差），
    // 实测 typedef 直调与该实调用指令数同为 148（+9 系函数既有 N82 帧差，非本转换引入）。
    if (event == 0 ||
        event->IsEventing(0) != '\x01')
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x138,
                              "[Taiwan, Event] stayEvent=%s eventing:%d",
                              (event == 0) ? "nullptr" : "ok",
                              event->IsEventing(0));
        return 0x139;
    }
    char rewardable = 0;
    if (!msg->m_received && event->IsRewardCondition())
    {
        rewardable = 1;
    }
    user->SetEventCreateDnfReward(msg->m_received);
    PacketGuard pg;
    pg.put_header(0, 0x22d);
    pg.put_short((unsigned short)event->GetRate());
    pg.put_byte(rewardable);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}

#include "TaiwanInternalDispatcher/InterSelectMobileAuthReward.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xc)  // ORIG @0x08162107 movl $0xc
{
}
#include "TaiwanInternalDispatcher/dtor_InterSelectMobileAuthReward.inc"
{
}

#include "TaiwanInternalDispatcher/InterSelectPcroomDailyReward.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xd)  // ORIG @0x08162179 movl $0xd
{
}
#include "TaiwanInternalDispatcher/dtor_InterSelectPcroomDailyReward.inc"
{
}

#include "TaiwanInternalDispatcher/InterUpdateP2pHolePunchingState.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xf)  // ORIG @0x081621eb movl $0xf
{
}
#include "TaiwanInternalDispatcher/dtor_InterUpdateP2pHolePunchingState.inc"
{
}

#include "TaiwanInternalDispatcher/InterInsertGiveBoxEventReward.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0x10)  // ORIG @0x0816225d movl $0x10
{
}
#include "TaiwanInternalDispatcher/dtor_InterInsertGiveBoxEventReward.inc"
{
}

#include "TaiwanInternalDispatcher/InterLoadHeroMissionEventData.inc"
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0x12)  // ORIG @0x081622cf movl $0x12
{
}
#include "TaiwanInternalDispatcher/dtor_InterLoadHeroMissionEventData.inc"
{
}

// ---- TaiwanInterfaceInterDispatcher 基类（ORIG W：C1 0x08161e32 / D1 0x08161e4a / D0 0x08161e70 / getCode 0x08161e8e）----

#include "TaiwanInternalDispatcher/TaiwanInterfaceInterDispatcher.inc"
    : m_code(code)
{
}
#include "TaiwanInternalDispatcher/dtor_TaiwanInterfaceInterDispatcher.inc"
{
}
#include "TaiwanInternalDispatcher/getCode.inc"
{
    return m_code;
}

#include "TaiwanInternalDispatcher/dtor_TaiwanInternalDispatcher.inc"
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), FnDelete());  // ORIG @0x08160212
}

// [b28c-31] N7 复核：虚调用 callee ORIG %edx vs 4.4 %eax（18=18，仅寄存器名差）。
#include "TaiwanInternalDispatcher/operator.inc"
    std::pair<const TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*>& item)
{
    /* N7 封顶（b26 NL26）：同 TaiwanDBDispatcher::FnDelete 判词。 */
    delete item.second;   // ORIG @0x08161e9a：test+je + vtable D0 调用
}

#include "TaiwanInternalDispatcher/getDispatcher.inc"
{
    std::map<TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*>::iterator it =
        m_dispatchers.find(code);
    if (it != m_dispatchers.end())
    {
        return it->second;
    }
    return 0;
}

#include "TaiwanInternalDispatcher/addDispatcher.inc"
                                             TaiwanInterfaceInterDispatcher* disp)
{
    if (getDispatcher(code) != 0)
    {
        return false;
    }
    m_dispatchers[code] = disp;
    return true;
}

#include "TaiwanInternalDispatcher/delDispatcher.inc"
{
    if (getDispatcher(code) != 0)
    {
        m_dispatchers.erase(m_dispatchers.find(code));
    }
}

// [b28d-solo-TaiwanInternalDispatcher] AE（10 链式块 + p=0；CreateDnfEventInfo ctor nothrow）
#include "TaiwanInternalDispatcher/init.inc"
{
    TaiwanInterfaceInterDispatcher* p = 0;
    p = new InterLoadServiceRestrictPolicy();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterLoadGeolocation();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterCheckStayTimeEvent();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectCreateDnfEventInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectCreateDnfUserInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectMobileAuthReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectPcroomDailyReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterUpdateP2pHolePunchingState();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterInsertGiveBoxEventReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterLoadHeroMissionEventData();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    return true;
}

// ============================================================================
// OnlyOrigBatch14：InterSelectMobileAuthReward::dispatch_sig
// ORIG T 0x0816132a（0x283）。buf 承载 Msg*。
// 注意：ORIG 0x08161384 处为无条件 jmp（E9 04 02 00 00 → +0x263），
// 发邮件主体（make_pair/findString/ReqDBSendNewSystemMultiMail/SendReward）
// 在 ORIG 中为不可达代码；执行路径仅回写 user+0x8ec31 = m_rewarded。
//
// 【N84 归因（batch-11 实测）】we-smaller 143 行 = ORIG 4.1 -O0 对
// `if ((msg->rewarded ^ 1) != 0 && <const-false>)` 型条件保留首项求值
// （0x08161378..58 movzbl/xor $1/test）+ 无条件 jmp + **文本发射不可达 body**
// （+0x79..+0x262：vector pair/findString×2/strncpy×2/9-loop/
// MakeSystemMultiMailPostal/ReqDBSendNewSystemMultiMail/GetRepeatEvent/SendReward
// + EH vector dtor cleanup）。gcc 4.4 -O0 对同形条件整体折叠（/tmp/ehtest.C
// 探针：`if(msg && (r^1) && 0)` 仅输出 tail+mov$0，首项求值与 body 全消）；
// volatile 守卫探针（ehtest2.C）可出 body 但条件三段 short-circuit 形距 ORIG
// `test;jmp` 更远、且需新增 TU static 符号（链接污染）+误启用发信行为风险。
// ⇒ 编译期不可构造，终判 N111（无入口死块，4.4 恒 DCE）；本函数保持「仅可达路径」
//   语义忠实实现，tail（0x0816158d）已逐条对齐（CUser.h m_field8ec31 bool 化代落后）。
// ============================================================================
// [b28d-solo-TaiwanInternalDispatcher] DIFF·N111 封顶（ORIG 0x08161384 无条件 jmp 后
// 0x5a..0x263 发邮件主体为无入口死块，4.4 恒 DCE；可达 tail 已逐条对齐，p167 3 MISSING 全在死块）
// [M2S6 2026-09-11 复核] ①ORIG 0x08161389 处 1 字节 0x00 使 objdump 顺序解码错位 ⇒ 报告
// orig_insns=166 含 13 条假指令（真实 158；被覆盖段实为 0x0816138a..a2 的
// lea/mov/call vector-pair-ctor + 两 movl）⇒ 工具口径 gap 恒含 +8 假差。
// ②恒 0 volatile bool 守卫实现 our=162 = 真实+4（movb 初始化 1 + movzbl/test/je 3）；
// 探针 /tmp/m2s6_probe3.o 三形实测：`if (A && 0)` 4.4 整体折叠连体删（f）、volatile 局部
// （f3，=现状 9 条）、`if (A && msg==0)` 复用已验非空事实（f，7 条）——均无 ORIG 的
// 「test; jmp + 保留体」单跳转形（4.1 -O0 发射不可达体、4.4 需 2 分支守卫）⇒ N111 维持。
// ③p167 余项全为 C1/C2·D1/D2 别名族（ORIG nm 同址：Inven_ItemC1≡C2@0x080cb854、
// vector<pair>D1≡D2@0x081349ea、pair C1≡C2@0x081627d4），非语义缺口。
#include "TaiwanInternalDispatcher/dispatch_sig_1.inc"
{
    Msg* msg = reinterpret_cast<Msg*>(buf);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__,
                              0x161, "[Taiwan, MobileAuth] interMsg is null.");
        return 0x162;
    }
    // ORIG 0x08161384 = 无条件 jmp（源码条件第二项编译期恒假），其后 0x08161389..0x0816158c
    // 的发邮件主体是 ORIG **文本发射的不可达代码**（0x204 字节）。4.4 前端把 `&& 0` 整体
    // 折叠并 DCE 该块 ⇒ 用恒 0 的局部 volatile 守卫迫使后端照常发射（局部量不产生符号；
    // 守卫恒 0 ⇒ 可观察行为与 ORIG 一致：该块永不执行）。
    volatile bool dead = false;
    if ((msg->m_rewarded ^ 1) != 0)
    {
        if (dead)
        {
            std::vector<std::pair<int, int> > items;
            items.push_back(std::pair<int, int>(0x28983a, 1));
            char title[0x1d];
            char mail[0xff];
            strncpy(title, g_scriptStringManager_.findString(4, "tw_mobile_authentication_title_01", 0), 0x1d);
            strncpy(mail, g_scriptStringManager_.findString(4, "tw_mobile_authentication_mail_01", 0), 0xff);
            Inven_Item invenItems[10];
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, invenItems, 10);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                title, invenItems, (unsigned int)items.size(), 0,
                (unsigned int)user->getCurCharacNo(), mail, (int)strlen(mail), 0,
                user->GetServerGroup(), false);
            CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xa1);
            if (ev != 0 && ev->IsEventing(0))
            {
                static_cast<CEventCeraShopRewardPoint*>(ev)->SendReward(user);
            }
        }
    }
    user->m_field8ec31 = msg->m_rewarded;
    return 0;
}

// ============================================================================
// OnlyOrigBatch14：InterUpdateP2pHolePunchingState::dispatch_sig
// ORIG T 0x08161952：P2P 洞穿状态包经统计代理转发（含国家代码）。
// ============================================================================
// 消息体读侧视图（§4.3）：ORIG 单段 disp `movzbl 0xd(%eax)`/`mov 0xe(%eax)`/
// `mov 0x12(%eax)`（0x081619xx）；宽=装载指令。
#pragma pack(push, 1)
struct P2pHoleMsgV
{
    char m_pad0[0xd];           // +0x00..未消费
    unsigned char m_connectedType;  // +0x0d
    int m_requiredTime;         // +0x0e
    int m_checkTime;            // +0x12
};
#pragma pack(pop)

// [b28d-solo-TaiwanInternalDispatcher] AE（proxy 具名局部去槽 + GetServerProxy()->SendPacket 链式）
#include "TaiwanInternalDispatcher/dispatch_sig_2.inc"
{
    // ORIG 0x08161952：统计包 → 国家代码 + 对端 IP → 经统计代理 UDP 转发。
    // ORIG 0x081619xx：buf 入口单槽物化 -0xc（无 char*+view 双中转，p 删除），
    // 三处字段读均经该槽 reload（技巧36 反向）。
    P2pHoleMsgV* v = reinterpret_cast<P2pHoleMsgV*>(buf);
    Packet_GameServer2Statisctics2DBServer pkt;
    pkt.m_serverGroup = (short)G_CEnvironment()->get_server_group();
    // ORIG `movzbl 0xd(%eax)`/`mov 0xe(%eax)`/`mov 0x12(%eax)` 单段 disp ⇒
    // pack(1) 视图具名字段（§4.3；裸 add 形实测不折 disp）。
    pkt.m_connectedType = v->m_connectedType;
    pkt.m_requiredTime = v->m_requiredTime;
    pkt.m_checkTime = v->m_checkTime;
    user->m_network.GetPeerIP2(pkt.m_peerAddress, 0x10);
    RestrictGeolocation::Get()->getCountryCode(
        pkt.m_nationCode, 0x10, std::string(pkt.m_peerAddress));
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)0)->SendPacket((char*)&pkt, 0x35);
    return 0;
}

// ============================================================================
// OnlyOrigBatch13 组 C —— 5 个 Inter dispatch_sig（ORIG 逐函数对照）
// ============================================================================
// df_game_r 还原 —— Inter 类 dispatch_sig 批次 C（5 个台湾域 Inter 分发函数）。
// 报告：docs/class_func_reports/{InterCheckStayTimeEvent,InterInsertGiveBoxEventReward,
//   InterLoadGeolocation,InterLoadHeroMissionEventData,InterLoadServiceRestrictPolicy}/dispatch_sig.md
// 语义逐指令对齐 ORIG（返回码 / 日志行号 / 调用序照抄反汇编）。

#include "TaiwanInterfaceInterDispatcher.h"
#include "CUser.h"                    // CUser / GetServerGroup / getCurCharacNo / IsFirstTimeLogin
#include "GlobalData.h"               // GlobalData::s_event_manager
#include "CGuildServerProxy.h"        // CEventManager::GetRepeatEvent / CEventBase
#include "CDataManager.h"             // G_CDataManager / find_item
#include "CItem.h"                    // CItem::make_item / get_index
#include "Inven_Item.h"               // Inven_Item::set_add_info
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "EventGiveMeBox.h"           // EventGiveMeBox::kRewardItem_
#include "CEventStayTime.h"           // CEventStayTime::kRewardItem_（ORIG 类静态）
#include "CEventStayTime.h"           // CEventStayTime::kRewardItem_（ORIG 类静态）
#include "RestrictGeolocation.h"      // RestrictGeolocation::addAllowCountry（含 ARAD::Singleton）
#include "ServiceRestrictManager.h"   // ServiceRestrictManager::beginPolicy / RestrictCategory
#include "HeroMissionEvent.h"         // HeroMissionEvent::setEventData/processMission/sendMissionData
#include "GameTypes.h"                // cMyTrace

#include <cstring>   // strlen
#include <string>

// ---- 载荷视图（pack(1)，按报告/反汇编真实布局；禁止裸偏移）----
#pragma pack(push, 1)
// InterLoadGeolocation：+0x00 countryCount(int)，+0x04 起每 16 字节一个 country code C 串
// （上限 0x2d 个，ORIG 反汇编 shl $0x4 步进实证）。
struct SigLoadGeolocation
{
    int countryCount;              // +0x00
    char countryCode[0x2d][16];    // +0x04
};

// InterLoadHeroMissionEventData：+0x00 头 4 字节，+0x04 起事件数据串（setEventData(buf+4)）。
struct SigLoadHeroMissionEventData
{
    char header[4];                // +0x00
    char data[1];                  // +0x04（C 字符串）
};

// InterLoadServiceRestrictPolicy：+0x00 category(int)，+0x119d 起策略串
// （ORIG 反汇编 lea 0x119d(%eax) 实证）。
struct SigLoadServiceRestrictPolicy
{
    int category;                  // +0x00
    char pad[0x119d - 0x04];       // +0x04 .. +0x119c
    char policyString[1];          // +0x119d（C 字符串）
};
#pragma pack(pop)

// ===== InterCheckStayTimeEvent::dispatch_sig —— ORIG 0x08160a42 =====
// buf==0 → cMyTrace(0xc9) 日志 "[Taiwan, StayEvent] SigStayTimeEvent is null." 返回 0xca；
// GetRepeatEvent(0x9f) 经 vtable+0x34 槽（ORIG IsEventing）判活，!=1 返回 0；
// 奖励道具 CEventStayTime::kRewardItem_E（0x2920908d）find_item 失败 → 日志(0xd3) 返回 0；
// 否则 make_item 填 Inven_Item、set_add_info(1)，取标题/正文脚本串，经
// ReqDBSendNewSystemMail(…, 0xe, group, false, false) 发系统邮件，返回 0。
// [b28d-solo-TaiwanInternalDispatcher] DIFF·N7+N18a 反向封顶（虚调 make_item 目标 %eax vs
// ORIG %edx；kRewardItem_ 装载单段 vs ORIG 两段；p167 16/16 CLEAR、gap 0）
// [M2S6 2026-09-11 复核] p167 余项 = C1/C2 别名族（ORIG Inven_ItemC1≡C2@0x080cb854、
// cMyTraceC1≡C2@0x0854f718）⇒ 非缺口；kRewardItem_ 装载经探针 /tmp/m2s6_probe2.o 证 4.4 对
// 静态成员地址恒单段直达 callee-saved（4.1 走 %eax 中转两段），N18a 维持；语义逐条核 ✓。
#include "TaiwanInternalDispatcher/dispatch_sig_3.inc"
{
    char* msg = buf;
    if (msg == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xc9, 5);
        tr("[Taiwan, StayEvent] SigStayTimeEvent is null.");
        return 0xca;
    }
    // ORIG 直调 vtable+0x34 槽（同 TaiwanInternalDispatcher.cpp 既有口径，保 ABI；无空检）。
    // §4.5 正解：真实虚调用 CEventBase::IsEventing(CUser*)@vtable+0x34（CGuildServerProxy.h 权威）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x9f)->IsEventing(0) != '\x01')
    {
        return 0;
    }
    CItem* item = G_CDataManager()->find_item(CEventStayTime::kRewardItem_);
    if (item == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xd3, 5)(
            "[Taiwan, StayEvent] Invalid item index. (%u)", CEventStayTime::kRewardItem_);
        return 0;
    }
    Inven_Item inv;
    inv.m_addInfo = item->get_index();   // ORIG 0x08160b54 mov %eax,-0x73（inv+0x02）
    item->make_item(inv);
    inv.set_add_info(1);
    const char* title = g_scriptStringManager_.findString(4, "tw_ccb_stress_title_01", 0);
    const char* mail = g_scriptStringManager_.findString(4, "tw_ccb_stress_mail_01", 0);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, inv, 0, (unsigned int)user->getCurCharacNo(), mail, (int)strlen(mail), 0xe,
        user->GetServerGroup(), false, false);
    return 0;
}

// ===== InterInsertGiveBoxEventReward::dispatch_sig —— ORIG 0x08161a8a =====
// 同 InterCheckStayTimeEvent 模式：event 0xa5、道具 EventGiveMeBox::kRewardItem_、
// 脚本串 tw_givemebox2_title/tw_givemebox2_mail；buf==0 → 日志(0x1dc) 返回 0x1dd。
// [b28d-solo-TaiwanInternalDispatcher] DIFF·N7+N18a 反向封顶（同 StayTime 形因；p167 16/16 CLEAR）
// [M2S6 2026-09-11 复核] 同 StayTime：p167 余项 = C1/C2 别名族（非缺口）；装载形 N18a 维持
// （探针 /tmp/m2s6_probe2.o）；语义（event 0xa5 / tw_givemebox2_* / 行号 0x1dc·0x1e6 / 返回 0x1dd）逐条核 ✓。
#include "TaiwanInternalDispatcher/dispatch_sig_4.inc"
{
    char* msg = buf;
    if (msg == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1dc, 5);
        tr("[Taiwan, GiveMeBoxEvent] msg is null.");
        return 0x1dd;
    }
    // §4.5 正解：真实虚调用 CEventBase::IsEventing(CUser*)@vtable+0x34（CGuildServerProxy.h 权威）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0xa5)->IsEventing(0) != '\x01')
    {
        return 0;
    }
    CItem* item = G_CDataManager()->find_item(EventGiveMeBox::kRewardItem_);
    if (item == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e6, 5)(
            "[Taiwan, GiveMeBoxEvent] Invalid item index. (%u)",
            EventGiveMeBox::kRewardItem_);
        return 0;
    }
    Inven_Item inv;
    inv.m_addInfo = item->get_index();   // ORIG 0x08161b9c mov %eax,-0x73（inv+0x02）
    item->make_item(inv);
    inv.set_add_info(1);
    const char* title = g_scriptStringManager_.findString(4, "tw_givemebox2_title", 0);
    const char* mail = g_scriptStringManager_.findString(4, "tw_givemebox2_mail", 0);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, inv, 0, (unsigned int)user->getCurCharacNo(), mail, (int)strlen(mail), 0xe,
        user->GetServerGroup(), false, false);
    return 0;
}

// ===== InterLoadGeolocation::dispatch_sig —— ORIG 0x0816088a =====
// buf==0 → cMyTrace(0xac) 日志 "[Taiwan, GeoIP] SigLoadRestrictPolicy is null." 返回 0xad；
// countryCount>0x2d → 日志(0xb2) 并写回 0x2d；循环 addAllowCountry 每国码（16 字节块），
// 每项 cMyTrace(0xb9,9,true,true) 日志 "GeoIP Allow Country Code : %s"；返回 0。
// [b28d-solo-TaiwanInternalDispatcher] AE（0x2du 无符号比较 + 两处 cMyTrace 临时链）
#include "TaiwanInternalDispatcher/dispatch_sig_5.inc"
{
    SigLoadGeolocation* sig = reinterpret_cast<SigLoadGeolocation*>(buf);
    if (sig == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xac, 5);
        tr("[Taiwan, GeoIP] SigLoadRestrictPolicy is null.");
        return 0xad;
    }
    if (sig->countryCount > 0x2du)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb2, 5)(
            "[Taiwan, GeoIP] Invalid GeoIP country count. (%d)", sig->countryCount);
        sig->countryCount = 0x2d;
    }
    for (int i = 0; i < sig->countryCount; ++i)
    {
        // ORIG：构造临时 string 直接值传（无拷贝），addAllowCountry 参数槽复用
        ARAD::Singleton<RestrictGeolocation>::Get()->addAllowCountry(
            std::string(sig->countryCode[i]));
        cMyTrace(__PRETTY_FUNCTION__, 0xb9, 9, true, true)(
            "GeoIP Allow Country Code : %s", sig->countryCode[i]);
    }
    return 0;
}

// ===== InterLoadHeroMissionEventData::dispatch_sig —— ORIG 0x08161c92 =====
// buf==0 → cMyTrace(0x1ff) 日志 "[HeroMission] msg is null." 返回 0x200；
// setEventData(user, buf+4)；IsFirstTimeLogin 命中 → processMission(user, 5, 0)；
// sendMissionData(user)；返回 0。
// [b28d-solo-TaiwanInternalDispatcher] AE（sig 提前 + Get() 链式 + IsFirstTimeLogin bool）
#include "TaiwanInternalDispatcher/dispatch_sig_6.inc"
{
    SigLoadHeroMissionEventData* sig =
        reinterpret_cast<SigLoadHeroMissionEventData*>(buf);
    if (sig == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1ff, 5);
        tr("[HeroMission] msg is null.");
        return 0x200;
    }
    ARAD::Singleton<HeroMissionEvent>::Get()->setEventData(user, sig->data);
    if (user->IsFirstTimeLogin())
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
            user, HeroMissionCondition::MissionNo::T_5, 0);
    }
    ARAD::Singleton<HeroMissionEvent>::Get()->sendMissionData(user);
    return 0;
}

// ===== InterLoadServiceRestrictPolicy::dispatch_sig —— ORIG 0x08160796 =====
// buf==0 → cMyTrace(0x94) 日志 "[Taiwan, Restrict] SigLoadRestrictPolicy is null." 返回 0x95；
// string 取 buf+0x119d，beginPolicy(Get(), *(int*)buf, string)；返回 0。
// [b28d-solo-TaiwanInternalDispatcher] AE（sig 提前 + std::string 临时全表达式）
#include "TaiwanInternalDispatcher/dispatch_sig_7.inc"
{
    SigLoadServiceRestrictPolicy* sig =
        reinterpret_cast<SigLoadServiceRestrictPolicy*>(buf);
    if (sig == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x94, 5);
        tr("[Taiwan, Restrict] SigLoadRestrictPolicy is null.");
        return 0x95;
    }
    ARAD::Singleton<ServiceRestrictManager>::Get()->beginPolicy(
        (RestrictCategory::Enum)sig->category, std::string(sig->policyString));
    return 0;
}
