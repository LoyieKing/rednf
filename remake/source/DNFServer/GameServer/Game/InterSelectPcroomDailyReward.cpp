// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InterSelectPcroomDailyReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— InterSelectPcroomDailyReward（台湾内部 PC 房每日奖励分发）。
// 报告：
//   makeRequest  docs/class_func_reports/InterSelectPcroomDailyReward/makeRequest.md（0x816179c）
//   dispatch_sig docs/class_func_reports/InterSelectPcroomDailyReward/dispatch_sig.md（0x81616ce）
//   SendPacket  docs/class_func_reports/InterSelectPcroomDailyReward/SendPacket.md（0x81618a6）
// 依赖：TaiwanInterfaceInterDispatcher（基类）实现由 OrigTaiwanDisp 落；
// CStreamGuard/StreamPool/MsgQueueMgr/Taiwan::internal_stream 已有权威实现。
#include "InterSelectPcroomDailyReward.h"

#include "CInventory.h"   // PacketGuard 完整定义（含 InterfacePacketBuf 基础方法）
#include "CStreamGuard.h"
#include "CUser.h"
#include "GlobalData.h"   // s_stream_pool / s_msgq_mgr
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "TaiwanInternal.h"  // Taiwan::internal_stream

// ---- SendPacket ----
// ORIG 0x81618a6：若 user==0 直接返回；否则 PacketGuard ctor;
//   put_header(0,0x22e); put_byte((char)user[0x8ec32]); finalize(true); CUser::Send。
//   0x8ec32 = CUser::m_field8ec32（char）。
#include "InterSelectPcroomDailyReward/SendPacket.inc"
{
    // b28c-33：ORIG 尾部为 `je <nop 岛>`（+0xa3）+ `jmp +0xa4`，即 P223 空 then 占位岛；
    // 正向 `if(user!=0){}` 被 4.4 折成直落 ⇒ 改负条件 goto 到共享尾空标签。
    if (user == 0)
        goto END;
    {
        PacketGuard guard;
        guard.put_header(0, 0x22e);
        guard.put_byte((int)(char)user->m_field8ec32);
        guard.finalize(true);
        user->Send(guard);
    }
END: ;
}

// ---- makeRequest ----
// ORIG 0x816179c：Acquire stream(0x1a8) → CStreamGuard(stream,true) →
// internal_stream(guard, 0xd, uid) → guard->GetInBuffer<Msg>()；
// 写 m_id(0xc)/remainCount(0x10 char) 入 MsgQueueMgr::put(1, guard)；
// msg==0 时 logFormat "[Taiwan, pcroom] m_id:%d" 后 return。
// 注意：__PRETTY_FUNCTION__ 实证第二参类型名为 memberIdentificationNumber_t（=unsigned int）。
#include "InterSelectPcroomDailyReward/makeRequest.inc"
                                               char remainCount)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x1a8),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xd), uid);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x1ad, "[Taiwan, pcroom] m_id:%d", m_id);
    }
    else
    {
        msg->m_id = m_id;
        msg->m_remainCount = remainCount;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}

// ---- dispatch_sig ----
// ORIG 0x81616ce。第二参 data 实际承载 Msg*（ORIG 汇编 0x10(%ebp)=data 判空；
// Ghidra C 的 param_3 标注为反编译器参数索引偏差，以汇编为准）。
// 流程：
//   data==0 → 日志 "[Taiwan, pcroom] interMsg is null."，返回 0x19b；
//   user->m_field8ec32 = msg->m_remainCount（movzbl 0x4 → mov %dl,0x8ec32）；
//   remain = movsbl msg->m_remainCount；idStr = NumberToString(msg->m_id, 0)；
//   cMyTrace(trace, 0x19f, 0) 日志 "[Taiwan, pcroom] m_id:%s, remain_reward_count:%d"；
//   SendPacket(user)，返回 0。
#include "InterSelectPcroomDailyReward/dispatch_sig.inc"
{
    Msg* msg = reinterpret_cast<Msg*>(data);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x19a, "[Taiwan, pcroom] interMsg is null.");
        return 0x19b;
    }
    user->m_field8ec32 = msg->m_remainCount;
    register int remain = (signed char)msg->m_remainCount;  // ORIG movsbl → esi
    register char* idStr = NumberToString(msg->m_id, 0);    // ORIG ebx 缓存
    cMyTrace trace("virtual int InterSelectPcroomDailyReward::dispatch_sig(CUser*, char*, int)",
                   0x19f, 0);
    trace("[Taiwan, pcroom] m_id:%s, remain_reward_count:%d", idStr, remain);
    SendPacket(user);
    return 0;
}
