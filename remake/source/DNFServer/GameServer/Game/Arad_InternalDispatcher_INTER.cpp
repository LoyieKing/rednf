// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_InternalDispatcher_INTER/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r Game/ ARAD::DISPATCHER INTER 流具体派发器（dispatch_sig 家族）。
//
// 依据 docs/class_func_reports/ARAD__DISPATCHER__Arad_INTER_*/ 各报告逐指令转写：
//   Arad_INTER_kAvatarRoulette            0x08198a9a
//   Arad_INTER_kLoadRewardEventItem       0x08198fd0
//   Arad_INTER_kSaveServerState           0x08199278
//   Arad_INTER_kLoadServerState           0x081992fe
//   Arad_INTER_kDeleteServerState         0x0819935e
//   Arad_INTER_kAradJumpingCharacter      0x081999f2
//     + clearEpicQuest                    0x081993c4
//     + sendHelperBox                     0x0819973e
//   Arad_INTER_kAvatarHiddenOptionChange  0x08199d6e
//   Arad_INTER_kUseAvatarRecharge         0x08199f0e
//   Arad_INTER_kEmblemCompound            0x0819a0c0
//   Arad_INTER_kAvatarConvert             0x0819a20e
//
// 本 TU 为唯一实现点（Arad_InternalDispatcher.cpp 已删对应空定义）；
// ctor/dtor 空体仍由 Arad_InternalDispatcher.cpp 承载。
// 包体字段访问：ORIG 偏移直读（MoreTypes.h Sig 布局归 OnlyOrig_ARAD 批）。
// ============================================================================
#include "Arad_InternalDispatcher.h"

#include <time.h>
#include <string.h>
#include <string>

#include "GlobalData.h"
#include "CUser.h"              // CUser::eSendTarget / UserQuest / quest getter
#include "CUserCharacInfo.h"
#include "CInventory.h"         // CInventory / OS_API::snprintf
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "AvatarCoin.h"
#include "DataManagerInitDepends.h"    // AvatarRouletteServer / LevelUpSkill
#include "CGuildServerProxy.h"  // CEventBase / CEventManager::GetRepeatEvent
#include "CSystemTime.h"
#include "CEnvironment.h"
#include "Quest.h"
#include "QuestClear.h"                 // WongWork::CQuestClear
#include "CDataManager.h"               // G_CDataManager / find_quest / m_aradJumping
#include "Arad_DataManager.h"           // ARAD::Singleton<T> / findGameScript
#include "Arad_EventPeriodDataManager.h"
#include "AradServerStateMessage.h"
#include "Arad_ServerStateManager.h"    // insertState/deleteState/ENUM_GLOBAL_EFFECT_ARAD
#include "AvatarFixedHiddenOptionServer.h"
#include "AvatarRechargeServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarConvertServer.h"
#include "MoreTypes.h"       // arad::SigAradEmblemCompound 权威定义（MoreTypes.h:257）

namespace
{

// ORIG INTER 包体字节视图（包字段布局归 OnlyOrig_ARAD 批的 Sig 结构化，
// 此处仅为指令级位移寻址形态保持：packet+offset 直读）。
#pragma pack(push, 1)
struct PktSaveServerStateView
{
    char m_padE[0xe];        // +0x00..0x0d
    int m_effect;            // +0x0e ENUM_GLOBAL_EFFECT_ARAD
    unsigned int m_endSec;   // +0x12
};
#pragma pack(pop)

}  // namespace

// ============================================================================
// AvatarRouletteServer::PickedAvatarInfo ctor —— ORIG W 0x0819a336
// （嵌套弱符号；dispatch_sig 栈对象需要，登记于本 TU）
// ============================================================================
// 类在全局命名空间（DataManagerInitDepends.h），无须 ns 限定。
#include "Arad_InternalDispatcher_INTER/PickedAvatarInfo.inc"
}

namespace ARAD
{

namespace DISPATCHER
{

// ============================================================================
// Arad_INTER_kLoadServerState —— ORIG 0x081992fe
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig.inc"
}

// ============================================================================
// Arad_INTER_kSaveServerState —— ORIG 0x08199278
//   栈上构造 AradServerStateMessage{m_active=1, m_endSec=*(pkt+0x12)}，
//   insertState(*(ENUM_GLOBAL_EFFECT_ARAD*)(pkt+0xe), msg)。
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_1.inc"
}

// ============================================================================
// Arad_INTER_kDeleteServerState —— ORIG 0x0819935e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_2.inc"
}

// ============================================================================
// Arad_INTER_kLoadRewardEventItem —— ORIG 0x08198fd0
//   user/CurCharac 空 → 0x4d3；sig 空 → log 0x4d8 返 0x4d9；
//   addUserRewardData → eventReward<FnRewardLethe>(0x8f) →
//   GetRepeatEvent(0x9b)->IsEventing(NULL) 命中再 eventReward<FnRewardMomoji>(0x9b)。
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_3.inc"
}

// ============================================================================
// Arad_INTER_kAradJumpingCharacter::clearEpicQuest —— ORIG 0x081993c4
//   第一遍扫角色 20 槽 in-progress 任务（等级达标即结清+发奖）；第二遍遍历
//   QuestList::getQuestNPCList(GRADE 0) 的 multimap value 对应任务，排除转职
//   0xe04..0xe08 且职业匹配、未清者结清+发奖。返回是否有任一结清。
// ============================================================================
#include "Arad_InternalDispatcher_INTER/clearEpicQuest.inc"
}

// ----------------------------------------------------------------------------
// sendHelperBox —— ORIG 0x0819973e：按 ticket 类型取标题/正文脚本串，经
// AradJumping_Script 取奖励物品后 create_item 并 ReqDBSendNewSystemMail 投递。
// ----------------------------------------------------------------------------
#include "Arad_InternalDispatcher_INTER/sendHelperBox.inc"
}

// ============================================================================
// Arad_INTER_kAradJumpingCharacter::dispatch_sig —— ORIG 0x081999f2
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_4.inc"
}

// ============================================================================
// Arad_INTER_kAvatarHiddenOptionChange —— ORIG 0x08199d6e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_5.inc"
}

// ============================================================================
// Arad_INTER_kUseAvatarRecharge —— ORIG 0x08199f0e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_6.inc"
}

// ============================================================================
// Arad_INTER_kEmblemCompound —— ORIG 0x0819a0c0
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_7.inc"
}

// ============================================================================
// Arad_INTER_kAvatarConvert —— ORIG 0x0819a20e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_8.inc"
}


// ============================================================================
// Arad_INTER_kAvatarRoulette —— ORIG 0x08198a9a
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_9.inc"
}

}  // namespace DISPATCHER

}  // namespace ARAD
