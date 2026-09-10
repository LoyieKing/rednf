// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NPCDynamicInfoManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// NPCDynamicInfoManager.cpp —— CNPCDynamicInfoManager::makeNotiPacketNPCMood /
// onTimer（权威实现归本 TU；ctor/dtor/getNPCInfo/add 归 CtorProducers_1.cpp）。
//
// ORIG 参照（nm -C df_game_r）：
//   _ZNK22CNPCDynamicInfoManager21makeNotiPacketNPCMoodER11PacketGuard
//                                                         T 0x085808a6
//   _ZN22CNPCDynamicInfoManager7onTimerEv                  T 0x08580722
// 汇编/反编译见 docs/class_func_reports/CNPCDynamicInfoManager/
//   makeNotiPacketNPCMood.md / onTimer.md
//
// makeNotiPacketNPCMood 结构：put_header(0,0xc3) → 记 index → put_short(0) 占位
// → 遍历 m_map（const_iterator，P13 缓存 end），逐 NPC makeNotiPacketCurMood
// 并计数 → put_short(index,count) 回写（P38 双 operator[] 拆语句同族）→
// finalize(true)。循环内 getMood() 调用一次但结果丢弃（ORIG 0x08581910 call +
// 存 -0xc，O0 无 CSE，P48）。
// ============================================================================

#include "NPCDynamicInfoManager.h"

#include "GameWorld.h"

// ============================================================================
// onTimer —— ORIG 0x08580722 T（range [0x08580722, 0x085808a5]）。
// 组包结构同 makeNotiPacketNPCMood；差异：循环内对每个 NPC 先取 mood，再
// changeMood(false)，仅当结果变化时 makeNotiPacketCurMood 并 ++count；
// 收尾 put_short(index,count) → finalize(true)；count!=0 才
// G_GameWorld()->send_all(guard)（ORIG +0x13e 起 call G_GameWorld/send_all）。
// 反编译见 docs/class_func_reports/CNPCDynamicInfoManager/onTimer.md。
// ============================================================================
#include "NPCDynamicInfoManager/onTimer.inc"
}

#include "NPCDynamicInfoManager/makeNotiPacketNPCMood.inc"
}
