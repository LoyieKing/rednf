// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NPCDynamicInfoManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
{
    // 声明序经实验对齐 ORIG 栈槽（count -0x12 / mood -0x10 / curMood -0xc /
    // guard -0x20 / idx -0x24 / it -0x28 / endIt -0x2c）。
    short count;
    ENUM_NPC_MOOD mood;
    ENUM_NPC_MOOD curMood;
    PacketGuard guard;
    int idx;

    guard.put_header(0, 0xc3);
    idx = guard.get_index();
    count = 0;
    guard.put_short(0);
    // 块内声明 + 声明即初始化：begin/end 直出 it/endIt 槽（无临时中转），
    // 且执行点落在 put_short(0) 之后，与 ORIG +0x055/+0x067 位置一致。
    std::map<unsigned int, CNPCDynamicInfo*>::iterator it = m_map.begin();
    std::map<unsigned int, CNPCDynamicInfo*>::iterator endIt = m_map.end();
    while (it != endIt) {
        mood = it->second->getMood();
        curMood = it->second->changeMood(false);
        if (mood != curMood) {
            it->second->makeNotiPacketCurMood(guard);
            ++count;
        }
        ++it;
    }
    guard.put_short(idx, count);
    guard.finalize(true);
    if (count != 0)
        G_GameWorld()->send_all(guard);
}

#include "NPCDynamicInfoManager/makeNotiPacketNPCMood.inc"
{
    guard.put_header(0, 0xc3);
    int index = guard.get_index();
    short count = 0;
    guard.put_short(0);
    std::map<unsigned int, CNPCDynamicInfo*>::const_iterator it = m_map.begin();
    std::map<unsigned int, CNPCDynamicInfo*>::const_iterator endIt = m_map.end();
    while (it != endIt) {
        int mood = it->second->getMood();
        it->second->makeNotiPacketCurMood(guard);
        ++count;
        ++it;
    }
    guard.put_short(index, count);
    guard.finalize(true);
}
