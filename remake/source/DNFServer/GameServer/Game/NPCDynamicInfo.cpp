// ============================================================================
// NPCDynamicInfo.cpp —— CNPCDynamicInfo 6 个 ORIG 方法（权威实现归本 TU）。
// 布局/头见 NPCDynamicInfo.h；ctor 归属 CharacterAIA2.cpp（W 0x0858191c）。
//
// ORIG 参照（nm -C df_game_r）：
//   changeMood 0x085804b2 T / forceChangeMood 0x0858053e T /
//   isKeyItem 0x08580584 T / giveGiftItem 0x085805a8 T /
//   makeNotiPacketCurMood 0x08580624 T / getMood 0x08581910 W（头内联）
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CNPCDynamicInfo10changeMoodEb`（ORIG 0x858186e）。
#include <iostream>


#include "NPCDynamicInfo.h"

#include "GlobalData.h"   // s_systemTime_（+0x941f714；changeMood 的 getCurSec this）
#include "PacketGuard.h"  // put_short / put_byte

// ORIG 0x085804b2 T：
//   flag 假时校验 m_fieldc <= getCurSec() 才更新；更新 = getNextMood 轮转 +
//   m_fieldc 累加当前秒。返回当前 mood（enum，直传 getMood eax）。
ENUM_NPC_MOOD CNPCDynamicInfo::changeMood(bool flag)
{
    if (flag || m_fieldc <= GlobalData::s_systemTime_.getCurSec()) {
        m_fieldc = m_script->getNextMood(m_moodState, m_field8);
        m_fieldc += GlobalData::s_systemTime_.getCurSec();
    }
    return getMood();
}

// ORIG 0x0858053e T：
//   getMood()==mood 直接 false；否则 while(changeMood(true)!=mood) 空循环，返回 true。
bool CNPCDynamicInfo::forceChangeMood(ENUM_NPC_MOOD mood)
{
    if (getMood() == mood)
        return false;
    while (changeMood(true) != mood)
        ;
    return true;
}

// ORIG 0x08580584 T：转发 CNPCScript::isKeyItem，bool 直传 eax（无规范化）。
bool CNPCDynamicInfo::isKeyItem(unsigned long key, unsigned int id) const
{
    return m_script->isKeyItem(key, id);
}

// ORIG 0x085805a8 T：
//   参数布局 (uint mood, ulong unk[保留], uint count, CItem const*, result&)；
//   局部 short r：r<=0 直返；否则 r *= getFavorRatePerMood(getMood())（filds/fmuls/fistps）。
unsigned short CNPCDynamicInfo::giveGiftItem(
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
void CNPCDynamicInfo::makeNotiPacketCurMood(PacketGuard& guard) const
{
    guard.put_short(m_script->getIndex());
    guard.put_byte((char)getMood());
}
