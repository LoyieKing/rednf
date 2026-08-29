// ============================================================================
// OnlyOrigBatch4_Force.cpp —— 本批仅 ORIG 符号的落地（构造/析构/operator== 族）。
//
// 内容：
//   1. T 外联定义：upgrade_table_t::MachineFrameInfo::MachineFrameInfo()
//      （ORIG T 0x089003b4：4×float 1.0f + int 2）。
//   2. W 外联定义（骨架头声明、本 TU 定义）：
//      MoneyLogPacket / LimitNpcBuyItemRequestInfo / PCK_AUCTION_(CLOSE|OPEN)
//      _PRIVATE_STORE_GA 的 ctor。
//   3. force 发射（P147/P149）：头内联值类的 C1/C2/D1/D2/C1ERKS/operator==，
//      显式构造/拷贝/析构一遍使编译器按 ODR 使用点发射同名 W 符号。
// ============================================================================

#include <string.h>

#include "MoneyLogPacket.h"
#include "LimitNpcBuyItemRequestInfo.h"
#include "PCK_AUCTION_CLOSE_PRIVATE_STORE_GA.h"
#include "PCK_AUCTION_OPEN_PRIVATE_STORE_GA.h"
#include "CItemAmplifier.h"         // upgrade_table_t::MachineFrameInfo
#include "CDungeonExpStatistic.h"
#include "CDungeonNode.h"           // CDungeonSuccessNode
#include "ScriptValue.h"            // ChangeStatusData
#include "CDataManager.h"           // CharacterKind/DimensionCoinInfo/LevelUpRewardItemData
#include "CDungeon.h"               // DimensionPartyCount
#include "EffectAnimationInfo.h"
#include "EquipmentParameterInfo.h" // ItemAuraInfo/EquipmentAuraAppendageData
#include "CEquipItem.h"             // ExpBonusPointData
#include "GoldCardEventStatistic.h"
#include "ItemFarmingData.h"
#include "CharacManageScript.h"     // ManagePointBonus
#include "Quest.h"                  // MonsterKillItem
#include "Packet_Broadcast_Msg.h"
#include "GameTypes.h"              // DynamicPool
#include "AvatarItem.h"             // WongWork::Avatar_Item
#include "CUser.h"                  // CerashopAddRestrict::InfoDaily
#include "Arad_DataManager.h"       // Arad_*::Data::operator==

// ============================================================================
// 1. upgrade_table_t::MachineFrameInfo（ORIG T 0x089003b4）
// ============================================================================
upgrade_table_t::MachineFrameInfo::MachineFrameInfo()
{
    m_f00 = 1.0f;
    m_f04 = 1.0f;
    m_f08 = 1.0f;
    m_f0c = 1.0f;
    m_i10 = 2;
}

// ============================================================================
// 2. W 外联 ctor 定义
// ============================================================================

// ORIG W 0x08694602（C2 同址）：PacketHeader(0x2723, 0x1e) + 5 int 清零。
MoneyLogPacket::MoneyLogPacket()
    : PacketHeader(0x2723, 0x1e)
{
    m_characNo = 0;
    m_money0 = 0;
    m_money1 = 0;
    m_accId = 0;
    m_money2 = 0;
}

// ORIG W 0x084b41fe（C2 同址）：仅 PacketHeader(0x27d8, 0x000a)。
LimitNpcBuyItemRequestInfo::LimitNpcBuyItemRequestInfo()
    : PacketHeader(0x27d8, 0x000a)
{
}

// ORIG W 0x085cb3dc（C2 同址）：PACKET_HEADER C1 → memset(this,0,0x16) →
// setCategory(0)/setPacketID(0xc)/setSize(0x16)。
PCK_AUCTION_CLOSE_PRIVATE_STORE_GA::PCK_AUCTION_CLOSE_PRIVATE_STORE_GA()
{
    memset(this, 0, 0x16);
    m_header.setCategory(0);
    m_header.setPacketID(0xc);
    m_header.setSize(0x16);
}

// ORIG W 0x085cb374（C2 同址）：PACKET_HEADER C1 → memset(this,0,0xaa) →
// setCategory(0)/setPacketID(0xb)/setSize(0xaa)。
PCK_AUCTION_OPEN_PRIVATE_STORE_GA::PCK_AUCTION_OPEN_PRIVATE_STORE_GA()
{
    memset(this, 0, 0xaa);
    m_header.setCategory(0);
    m_header.setPacketID(0xb);
    m_header.setSize(0xaa);
}

// ============================================================================
// 3. force 发射（匿名命名空间，防外部链接）
// ============================================================================
namespace
{

// 头内联 W 值类的显式构造/拷贝/析构（生成 ORIG 同名 W 符号）。
void forceEmitBatch4()
{
    // CDungeonExpStatistic（头内联 ctor：map 默认构造）
    CDungeonExpStatistic s0;
    (void)s0;

    // CDungeonSuccessNode（头内联拷贝构造：两 map 拷贝）
    CDungeonSuccessNode a1;
    CDungeonSuccessNode b1(a1);
    (void)b1;

    // ChangeStatusData（头内联拷贝构造 + operator=，P147 force 发射
    // _ZN16ChangeStatusDataC1ERKS_ / _ZN16ChangeStatusDataaSERKS_）
    ChangeStatusData c1;
    ChangeStatusData c2(c1);
    c1 = c2;
    (void)c1;

    // CharacterKind（头内联 ctor：两字节清零）
    CharacterKind d1;
    (void)d1;

    // DimensionCoinInfo（头内联 ctor：int+bool 清零）
    DimensionCoinInfo e1;
    (void)e1;

    // DimensionPartyCount（头内联 ctor：三字节赋值）
    DimensionPartyCount f1((char)1, (char)2, (char)3);
    (void)f1;

    // EffectAnimationInfo（头内联 C1/D1）
    EffectAnimationInfo g1;
    (void)g1;

    // EquipmentAuraAppendageData（头内联空析构）
    EquipmentParameterInfo::EquipmentAuraAppendageData h1;
    (void)h1;

    // ExpBonusPointData（头内联 ctor：调 clear）
    ExpBonusPointData i1;
    (void)i1;

    // GoldCardEventStatistic（头内联 ctor）
    GoldCardEventStatistic j1;
    (void)j1;

    // ItemAuraInfo（头内联 ctor）
    ItemAuraInfo k1;
    (void)k1;

    // ItemFarmingData（头内联 C1）
    ItemFarmingData l1;
    (void)l1;

    // LevelUpRewardItemData（头内联 ctor：调 clear）
    LevelUpRewardItemData m1;
    (void)m1;

    // ManagePointBonus（头内联 ctor：三 int 清零）
    ManagePointBonus n1;
    (void)n1;

    // MonsterKillItem（头内联 ctor：调 clear）
    MonsterKillItem o1;
    (void)o1;

    // Packet_Broadcast_Msg（头内联 ctor：PacketHeader(0x2711, 0x10e)）
    Packet_Broadcast_Msg p1;
    (void)p1;

    // DynamicPool<WongWork::Avatar_Item>（模板 ctor，ORIG W 0x082fa3fc）
    DynamicPool<WongWork::Avatar_Item> q1(16, 16);
    (void)q1;

    // CerashopAddRestrict::InfoDaily（头内联 ctor：7 参）
    CerashopAddRestrict::InfoDaily r1(CerashopAddRestrict::RestrictType::RestrictType_Account,
                                      0u, 0u, 0L, 0L, 0, 0);
    (void)r1;

    // Arad 两个 Data::operator==（头内联，std::find 语义）
    Arad_OnePlusCardEvent_ItemList::Data s1((unsigned short)1, 2ul);
    Arad_OnePlusCardEvent_ItemList::Data s2((unsigned short)1, 3ul);
    bool sb = (s1 == s2);
    (void)sb;
    Arad_VillageAttackRewardItem::Data t1(10, 20ul, 30);
    Arad_VillageAttackRewardItem::Data t2(10, 20ul, 30);
    bool tb = (t1 == t2);
    (void)tb;
}

}  // namespace
