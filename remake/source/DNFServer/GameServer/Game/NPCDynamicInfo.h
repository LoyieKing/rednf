#ifndef GAME_NPCDYNAMICINFO_H
#define GAME_NPCDYNAMICINFO_H

// ============================================================================
// CNPCDynamicInfo —— NPC 动态信息（完整类型归 NPC 动态域，本头为权威布局）。
//
// 布局（ORIG ctor 0x0858191c 实证）：sizeof 0x10
//   +0x00 const CNPCScript* m_script    （ctor: =script）
//   +0x04 int m_moodState               （ctor: =3）
//   +0x08 int m_field8                  （ctor: =0x7fffffff）
//   +0x0c int m_fieldc                  （ctor: =0）
// ctor 定义归属 CharacterAIA2.cpp（W 0x0858191c）；本头仅声明，供本域成员函数使用。
//
// ORIG 符号（nm -C df_game_r，2026-08-28）：
//   _ZN15CNPCDynamicInfo10changeMoodEb                   T 0x085804b2
//   _ZN15CNPCDynamicInfo15forceChangeMoodE13ENUM_NPC_MOOD T 0x0858053e
//   _ZNK15CNPCDynamicInfo9isKeyItemEmj                   T 0x08580584
//   _ZNK15CNPCDynamicInfo12giveGiftItemEjmjPK5CItemR32ENUM_NPC_FAVOR_GIFT_RESULT_CONST
//                                                         T 0x085805a8
//   _ZNK15CNPCDynamicInfo21makeNotiPacketCurMoodER11PacketGuard T 0x08580624
//   _ZNK15CNPCDynamicInfo7getMoodEv                      W 0x08581910（内联）
// ============================================================================

#include "NPCScript.h"

class CItem;
class PacketGuard;

class CNPCDynamicInfo
{
public:
    // [b28d-Main 代落·S139 N100] 加 throw()：ORIG C1 为 nothrow（CNPCDynamicInfoManager::add
    // 的 new-expression 无 landing pad/operator delete 清理路径）；仅标量赋值，
    // 异常规范不改变 mangled 名/sizeof/布局。
    CNPCDynamicInfo(const CNPCScript* script) throw();

    // ORIG 0x085804b2 T：条件更新 mood；返回当前 mood（enum，直传 getMood）。
    ENUM_NPC_MOOD changeMood(bool flag);
    // ORIG 0x0858053e T：循环 changeMood(true) 直至 mood 命中。
    bool forceChangeMood(ENUM_NPC_MOOD mood);
    // ORIG 0x08580584 T：转发 CNPCScript::isKeyItem（bool 直传 eax）。
    bool isKeyItem(unsigned long key, unsigned int id) const;
    // ORIG 0x085805a8 T：转发 giveGiftItem 后按当前 mood 倍率加权（short 局部）。
    unsigned short giveGiftItem(unsigned int mood, unsigned long unk, unsigned int count,
                                CItem const* item,
                                ENUM_NPC_FAVOR_GIFT_RESULT_CONST& result) const;
    // ORIG 0x08580624 T：index short + mood byte 写入包。
    void makeNotiPacketCurMood(PacketGuard& guard) const;

    // ORIG _ZNK15CNPCDynamicInfo7getMoodEv W 0x08581910：
    // mov 0x4(%eax),%eax —— 直返 m_moodState（P32）。
    ENUM_NPC_MOOD getMood() const { return m_moodState; }

private:
    const CNPCScript* m_script;   // +0x00
    ENUM_NPC_MOOD m_moodState;    // +0x04（getNextMood 首参引用；getMood 直返）
    unsigned int m_field8;        // +0x08（getNextMood 次参引用）
    int m_fieldc;                 // +0x0c（changeMood：getNextMood(int) 返回直存，直读比较）
};

#endif  // GAME_NPCDYNAMICINFO_H
