// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OnlyOrigBatch4_Force/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
}

// ============================================================================
// 2. W 外联 ctor 定义
// ============================================================================

// ORIG W 0x08694602（C2 同址）：PacketHeader(0x2723, 0x1e) + 5 int 清零。
#include "OnlyOrigBatch4_Force/MoneyLogPacket.inc"
}

// ORIG W 0x084b41fe（C2 同址）：仅 PacketHeader(0x27d8, 0x000a)。
#include "OnlyOrigBatch4_Force/LimitNpcBuyItemRequestInfo.inc"
}

// ORIG W 0x085cb3dc（C2 同址）：PACKET_HEADER C1 → memset(this,0,0x16) →
// setCategory(0)/setPacketID(0xc)/setSize(0x16)。
#include "OnlyOrigBatch4_Force/PCK_AUCTION_CLOSE_PRIVATE_STORE_GA.inc"
}

// ORIG W 0x085cb374（C2 同址）：PACKET_HEADER C1 → memset(this,0,0xaa) →
// setCategory(0)/setPacketID(0xb)/setSize(0xaa)。
#include "OnlyOrigBatch4_Force/PCK_AUCTION_OPEN_PRIVATE_STORE_GA.inc"
}

// ============================================================================
// 3. force 发射（匿名命名空间，防外部链接）
// ============================================================================
namespace
{

// 头内联 W 值类的显式构造/拷贝/析构（生成 ORIG 同名 W 符号）。
#include "OnlyOrigBatch4_Force/forceEmitBatch4.inc"
}

}  // namespace
