// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OnlyOrigBatch5_Force/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// OnlyOrigBatch5_Force.cpp —— 本批仅 ORIG 符号的落地（构造/析构/拷贝族）。
//
// 内容：
//   1. force 发射（P147）：头内联值类的 C1/C2/D1/D2/C1ERKS，显式构造/拷贝/
//      析构一遍使编译器按 ODR 使用点发射同名 W 符号。
//   2. Packet_* 18 个 ctor、PassiveObjGenerator* 3 个拷贝构造、
//      PostalReward 拷贝构造、PvPSkillTreeKey 3 参 ctor、
//      QuantityRatio/ResetValue/PartSelectonField 空析构、
//      RandomApplicationKey 默认 ctor、SIG_* 8 个 ctor。
// 布局/语义均按 ORIG 反汇编逐符号核对（见报告 /tmp/df_batches/OnlyOrigBatch5.md）。
// ============================================================================

#include <string.h>

#include "../../../shared/packet/include/Packet_DeathTower_Statistic_Value.h"
#include "../../../shared/packet/include/Packet_Forbid_Chat_By_Monitor.h"
#include "../../../shared/packet/include/Packet_GameServer2Statisctics2DBServer.h"
#include "../../../shared/packet/include/Packet_Guild_Check_Guild_Cargo_Push_Item.h"
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Move_Item.h"
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Pop_Item.h"
#include "../../../shared/packet/include/Packet_HellParty_Statistic_Item.h"
#include "../../../shared/packet/include/Packet_Monitor_Guild_Chat.h"
#include "../../../shared/packet/include/Packet_Monitor_MegaPhone.h"
#include "../../../shared/packet/include/Packet_Monitor_Member_Chat.h"
#include "../../../shared/packet/include/Packet_Monitor_Notice_Message.h"
#include "../../../shared/packet/include/Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"
#include "../../../shared/packet/include/Packet_P2P_Statistics.h"
#include "../../../shared/packet/include/Packet_Pcroom_Response.h"
#include "../../../shared/packet/include/Packet_PvPResult.h"
#include "../../../shared/packet/include/Packet_User_Ting_TimeCheck_Statistic_Add.h"
#include "../../../shared/packet/include/Packet_Value_Statistic.h"
#include "../../../shared/packet/include/Packet_VillageMonsterFightResult.h"

#include "W8LT_PassiveObjectScript.h"  // PassiveObjGeneratorObject/Pattern/Sequence
#include "Quest.h"                     // PostalReward
#include "CDataManager.h"              // PvPSkillTreeKey
#include "RandomOptionScript.h"        // RandomApplicationKey/QuantityRatio/ResetValue/PartSelectonField
#include "SigTypes.h"                  // SIG_* 全部分片

// ============================================================================
// force 发射（匿名命名空间，防外部链接）
// ============================================================================
namespace
{

#include "OnlyOrigBatch5_Force/forceEmitBatch5.inc"
}

}  // namespace
