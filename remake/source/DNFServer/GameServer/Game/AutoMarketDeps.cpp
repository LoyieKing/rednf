// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AutoMarketDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次：CAutoMarketConditionsControl 单用户耐久通知。
// ORIG 依据：报告 CAutoMarketConditionsControl/SendNotiSetDurability_082f85ea.md
//   _ZN28CAutoMarketConditionsControl20SendNotiSetDurabilityEP5CUser T 0x082f85ea
#include "CDataManager.h"   // CAutoMarketConditionsControl 权威类（CDataManager.h 内声明）

#include "CUser.h"          // CUser::Send(PacketGuard&)
#include "PacketGuard.h"

// MakeNotiSetDurability —— ORIG T 0x082f84f8（LinkMiscSecuTimerSolo.cpp 定义）。
#include "AutoMarketDeps/MakeNotiSetDurability.inc"

// ORIG 0x082f85ea：m_field3e8(+0x3e8)==0 直接返回；否则
// MakeNotiSetDurability(packet, (int)(m_durCurrent(+0x43c double) * 1000.0))
// （常量 1000.0 = _DAT_08c1fc58，.rodata 双精度 1000.0），CUser::Send。
#include "AutoMarketDeps/SendNotiSetDurability.inc"
{
    // b28c-33：ORIG then 块尾直落 epilogue（无 jmp/nop 岛）⇒ 正向包裹形；
    // 早返形使 4.4 多发 `jmp <epilogue>; nop`（+2 条）。
    if (m_field3e8 != 0)
    {
        PacketGuard packet;
        MakeNotiSetDurability(packet,
                              static_cast<int>(m_durCurrent * 1000.0));
        user->Send(packet);
    }
}
