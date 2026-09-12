#ifndef GAME_DNFITEMINFO_H_
#define GAME_DNFITEMINFO_H_

// 布局依据 ORIG 反汇编（df_game_r W 符号：ctor 0x84e910a / reset 0x84e9094）：
//   pack(1)，sizeof 0x35。+0x13..0x1c = 强化 guid（stAmplifyGuidView 0xa；ORIG reset
//   不触碰该区，由 dispatch_sig 从 item.m_amp.m_guid 拷入 —— 见下方 m_guid 处证据）。
//   +0x1d RandomOption（Inven_Item.h 权威，0xe 字节）+0x2b UpgradeSeparateInfo
//   +0x2c ReservedCapacity。字段命名/布局与 Guild/GuildCargo.h 权威版一致。
#include "Inven_Item.h"

#pragma pack(push,1)
class DnfItemInfo
{
public:
    // ORIG 0x84e910a _ZN11DnfItemInfoC1Ev：m_up@+0x2b/m_res@+0x2c 成员 ctor 后调 reset()
    DnfItemInfo();
    // ORIG 0x84e9094 _ZN11DnfItemInfo5resetEv
    void reset();

    unsigned char m_seal;          // +0x00
    unsigned int m_itemId;         // +0x01
    unsigned char m_attr;          // +0x05
    unsigned int m_addInfo;        // +0x06
    unsigned short m_endurance;    // +0x0a
    unsigned int m_extendInfo;     // +0x0c
    unsigned char m_abilityType;   // +0x10
    unsigned short m_abilityValue; // +0x11
    // [PadFix 2026-09-12 §4.3/§8.3] 原 `char m_pad[0x1d - 0x13]` 占位拆为具名字段。
    // 证据（ORIG 逐指令，Inter_AuctionResultAskRegistedItemNum::dispatch_sig；
    // 报告 docs/class_func_reports/Inter_AuctionResultAskRegistedItemNum/dispatch_sig.md）：
    //   0x084d66d9 `mov -0xa1(%ebp),%eax; mov %eax,-0x138(%ebp)`  item+0x15 → pck+0x43
    //   0x084d66e5 `mov -0x9d(%ebp),%eax; mov %eax,-0x134(%ebp)`  item+0x19 → pck+0x47
    //   0x084d66f1 `movzwl -0x99(%ebp),%eax; mov %ax,-0x130(%ebp)` item+0x1d → pck+0x4b（16 位）
    //   （pck=-0x17b(%ebp)，item_info@pck+0x30 ⇒ 目标恰为 item_info+0x13/+0x17/+0x1b；
    //    源 item+0x15/0x19/0x1d = Inven_Item::m_amp@+0x11 的 +0x04/+0x08/+0x0c
    //    = stAmplifyGuidView{m_serverId,m_datetime,m_sequenceNo}，4+4+2 恰 0xa）
    //   ⇒ 该 10 字节语义 = 强化 guid；其后 +0x1d 起才是 RandomOption（0x084d66ff..0x084d672a）。
    stAmplifyGuidView m_guid;      // +0x13..+0x1c（0xa，CItemAmplifier.h 权威类型）
    RandomOption m_opt;            // +0x1d（0xe 字节）
    UpgradeSeparateInfo m_up;      // +0x2b
    ReservedCapacity m_res;        // +0x2c
};
#pragma pack(pop)

// [PadFix 2026-09-12] 字段化前后 sizeof/偏移不变（m_pad[0xa] → stAmplifyGuidView 0xa）。
static_assert(sizeof(DnfItemInfo) == 0x35, "DnfItemInfo size drift");
static_assert(offsetof(DnfItemInfo, m_guid) == 0x13, "DnfItemInfo m_guid offset drift");
static_assert(offsetof(DnfItemInfo, m_opt) == 0x1d, "DnfItemInfo m_opt offset drift");

#endif  // GAME_DNFITEMINFO_H_
