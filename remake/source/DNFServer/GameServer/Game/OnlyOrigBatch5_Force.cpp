// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch5_Force/<Func>.inc（不参与编译、内部不含 #include）。
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


}  // namespace
