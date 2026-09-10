// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch4_Force/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>
#include <new>   // b29：placement new（CDungeonExpStatistic 只发 ctor 不发 dtor）

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
#include "OnlyOrigBatch4_Force/MachineFrameInfo.inc"
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
#include "OnlyOrigBatch4_Force/MoneyLogPacket.inc"
    : PacketHeader(0x2723, 0x1e)
{
    m_characNo = 0;
    m_money0 = 0;
    m_money1 = 0;
    m_accId = 0;
    m_money2 = 0;
}

// ORIG W 0x084b41fe（C2 同址）：仅 PacketHeader(0x27d8, 0x000a)。
#include "OnlyOrigBatch4_Force/LimitNpcBuyItemRequestInfo.inc"
    : PacketHeader(0x27d8, 0x000a)
{
}

// ORIG W 0x085cb3dc（C2 同址）：PACKET_HEADER C1 → memset(this,0,0x16) →
// setCategory(0)/setPacketID(0xc)/setSize(0x16)。
#include "OnlyOrigBatch4_Force/PCK_AUCTION_CLOSE_PRIVATE_STORE_GA.inc"
{
    memset(this, 0, 0x16);
    m_header.setCategory(0);
    m_header.setPacketID(0xc);
    m_header.setSize(0x16);
}

// ORIG W 0x085cb374（C2 同址）：PACKET_HEADER C1 → memset(this,0,0xaa) →
// setCategory(0)/setPacketID(0xb)/setSize(0xaa)。
#include "OnlyOrigBatch4_Force/PCK_AUCTION_OPEN_PRIVATE_STORE_GA.inc"
{
    memset(this, 0, 0xaa);
    setCategory(0);     // 继承 nsl::PACKET_HEADER（m_header 成员已归一为基类）
    setPacketID(0xb);
    setSize(0xaa);
}

// ============================================================================
// 3. force 发射（匿名命名空间，防外部链接）
// ============================================================================
namespace
{

// 头内联 W 值类的显式构造/拷贝/析构（生成 ORIG 同名 W 符号）。
#include "OnlyOrigBatch4_Force/forceEmitBatch4.inc"
{
    // CDungeonExpStatistic（头内联 ctor：map 默认构造）。ORIG 仅发 C1/C2
    // （W 0x082aa88a），无 D1/D2 ⇒ placement 构造触发 ctor ODR-use，对象不
    // 析构 ⇒ 不发射 dtor（栈对象会多发明 D1/D2）。
    char expStatStorage[sizeof(CDungeonExpStatistic)]
        __attribute__((aligned(8)));
    new (expStatStorage) CDungeonExpStatistic;

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
