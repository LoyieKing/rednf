// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NPCDynamicInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CNPCDynamicInfo10changeMoodEb`（ORIG 0x858186e）。
// [AnonLane round8 2026-09-11] `._199`（ORIG 在本 TU 段的 m_fav 匿名副本）编号垫开关：
// 本 TU 私有宏 ⇒ 启用 NpcScript.h 内对应的 `#ifdef FAV_ANON_199_EXTRA` 哑匿名类型块
// （把本 TU 的 m_fav 编号垫到 199），其它 TU 不受影响。
#define FAV_ANON_199_EXTRA 1
#include <iostream>


#include "NPCDynamicInfo.h"

#include "GlobalData.h"   // s_systemTime_（+0x941f714；changeMood 的 getCurSec this）
#include "PacketGuard.h"  // put_short / put_byte

// ORIG 0x085804b2 T：
//   flag 假时校验 m_fieldc <= getCurSec() 才更新；更新 = getNextMood 轮转 +
//   m_fieldc 累加当前秒。返回当前 mood（enum，直传 getMood eax）。
#include "NPCDynamicInfo/changeMood.inc"
{
    if (flag || m_fieldc <= GlobalData::s_systemTime_.getCurSec()) {
        m_fieldc = m_script->getNextMood(m_moodState, m_field8);
        m_fieldc += GlobalData::s_systemTime_.getCurSec();
    }
    return getMood();
}

// ORIG 0x0858053e T：
//   getMood()==mood 直接 false；否则 while(changeMood(true)!=mood) 空循环，返回 true。
#include "NPCDynamicInfo/forceChangeMood.inc"
{
    if (getMood() == mood)
        return false;
    while (changeMood(true) != mood)
        ;
    return true;
}

// ORIG 0x08580584 T：转发 CNPCScript::isKeyItem，bool 直传 eax（无规范化）。
#include "NPCDynamicInfo/isKeyItem.inc"
{
    return m_script->isKeyItem(key, id);
}

// ORIG 0x085805a8 T：
//   参数布局 (uint mood, ulong unk[保留], uint count, CItem const*, result&)；
//   局部 short r：r<=0 直返；否则 r *= getFavorRatePerMood(getMood())（filds/fmuls/fistps）。
#include "NPCDynamicInfo/giveGiftItem.inc"
    unsigned int mood, unsigned long unk, unsigned int count,
    CItem const* item, ENUM_NPC_FAVOR_GIFT_RESULT_CONST& result) const
{
    short r = m_script->giveGiftItem(mood, item, count, result);
    if (r > 0) {
        r = (short)((float)r * m_script->getFavorRatePerMood(getMood()));
    }
    return r;
}

// ORIG 0x08580624 T：put_short(getIndex()) + put_byte((char)getMood())。
#include "NPCDynamicInfo/makeNotiPacketCurMood.inc"
{
    guard.put_short(m_script->getIndex());
    guard.put_byte((char)getMood());
}

// [AnonLane round8 2026-09-11] `._199` 发射点（与 GiantB `DisjointScript::._381` 同法）：
// ORIG 的 `_ZN25stFavorableRelationShip_t5._199D1/D2Ev`（t 0x0858046c，各 sz 0x45）由**本 TU
// 段**发射（0x0858xxxx：与 `~stFavorableRelationShip_t`@0x08581ae0、
// `CNPCDynamicInfo::changeMood`@0x085804b2 相邻；GiantB 逐指令核出调用者即该 TU 的宿主析构
// 0x08581d39/0x08581d42 `add $0x10,%eax; call ._199D1Ev`）。
// `delete p` **只调析构**（不构造）⇒ 发射 D1/D2、不发 C1/C2，与 ORIG 的符号集合一致；
// p 恒为空指针也走析构路径（符号必发）。语义/布局零影响。
void __favAnon199ForceClear(stFavorableRelationShip_t* p)
{
    delete p;
}
