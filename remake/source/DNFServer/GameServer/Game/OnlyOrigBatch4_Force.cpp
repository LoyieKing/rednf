#define _GLIBCXX_IOSTREAM 1
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

#include "OnlyOrigBatch4_Force/MachineFrameInfo.inc"


// ============================================================================
// 2. W 外联 ctor 定义
// ============================================================================

#include "OnlyOrigBatch4_Force/MoneyLogPacket.inc"


#include "OnlyOrigBatch4_Force/LimitNpcBuyItemRequestInfo.inc"


#include "OnlyOrigBatch4_Force/PCK_AUCTION_CLOSE_PRIVATE_STORE_GA.inc"


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

#include "OnlyOrigBatch4_Force/forceEmitBatch4.inc"


}  // namespace
