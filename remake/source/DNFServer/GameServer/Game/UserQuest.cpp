// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 UserQuest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r UserQuest（G2-5 批次）逐函数还原
// 语义依据：docs/class_func_reports/UserQuest.md（汇编为准）+ ORIG 反汇编直读。
// 布局（由 ORIG ctor/dtor 推导，总大小 0x761d pack(1)）：
//   +0x0000 CUser* m_user
//   +0x0004 WongWork::CQuestClear m_cleared（0x7530）
//   +0x7538 int m_quest[20]、+0x7588 int m_trigger[20]
//   +0x75d8 _Quest_Authen_Data（0x44：characNo/characClass/…/urgentQuest/authen[]）
//   +0x761c bool m_field761c（0x761d = CUser.h 的 m_pcRoomAuth 起点）
// 本 TU 定义 UserQuest 全部方法（构造/析构/23 个报告函数 + set_parent），
// 跨类调用均为真实类型 C++ 调用（无 asm 桥）。
// ============================================================================

#include <cstring>
#include <list>
#include <map>
#include <vector>

// ---- 权威头 ----
#include "CDataManager.h"      // CDataManager / QuestList / ENUM_QUEST_GRADE
#include "Quest.h"             // 全局 Quest（权威 0x150 布局；替代本 TU 旧局部视图）
#include "CUserCharacInfo.h"   // CUserCharacInfo（CUser 基类）权威声明
#include "CUser.h"              // 全局权威 CUser / UserQuest（本 TU 实现其方法）
#include "QuestClear.h"        // WongWork::CQuestClear / STClearedQuest
#include "InterfacePacketBuf.h"
#include "CTimeGate.h"
#include "GlobalData.h"      // s_timeGate_（get_quest_info 经 ORIG `mov 0x941f7f4` 读指针）
#include "CPowerManager.h"
#include "GameTypes.h"         // cMyTrace

// ============================================================================
// 本地最小视图（权威声明见 CUser.h / CInventory.h / CBattle_Field.h；因这些头
// 内置最小 UserQuest 与本 TU 完整 UserQuest 冲突，暂不 include —— 与
// CAchievement.cpp 同款模式；方法定义在权威 TU，mangled 与 ORIG 一致）
// ============================================================================




// ---- ORIG 域常量（nm：_ZN21UrgentQuestLevelLimitL…E，r 段）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
namespace UrgentQuestLevelLimit
{
static const int kStartLvl = 27;   // 0x08ae5d64 (0x1b)
static const int kEndLvl = 70;     // 0x08ae5d68 (0x46)
}

// [Lane37CUser 2026-09-10 解环连带] 原 TU 局部 `class CInventory` 镜像已删：解环后
//   CUser.h 反向 include CInventory.h ⇒ 同 TU 内镜像与权威定义冲突
//   （UserQuest.cpp:50 redefinition vs CInventory.h）。改 include 权威头（方法签名
//   齐备：erase_repeated_item/CalNeedSlot/QuickEmptyCount/InvenEmptycount/
//   insert_event_items）。
#include "CInventory.h"
class CBattle_Field
{
public:
    void* getMaze();  // _ZN13CBattle_Field7getMazeEv（权威见 CBattle_Field.h，因含 CUser.h 未引入）
};

// getMaze() 返回迷宫对象 +0x50 处的 quest idx 字段视图（giveup_quest ORIG
// 单指令 mov 0x50(%eax) 折叠需要组件视图）
struct MazeQuestView
{
    char m_pad[0x50];  // +0x00
    int m_questIdx;    // +0x50
};

// stSelectQuestParam 前 11 个 int 的数组视角（P311/P183）：ORIG accept_quest
// 0x086ac234/0x086ac243 为单条 `mov -0xb0(%ebp,%eax,4),%eax`（基址+scaled），
// 用成员数组视图免 shl+add 且不缓存基址；偏移与权威成员逐一相同（m_level@+0）。
struct SelectQuestParamDataView
{
    int m_data[11];   // +0x00..+0x2b
};

// ---- stQuestTriggerState_t（8 字节：3 个 9 位值；方法定义在 QuestTriggerState.cpp）----


// Quest 类型改用 Quest.h 全局权威类（0x150；本 TU 旧局部视图字段名映射：
//   m_levelLimit→m_levelMax、m_rewardItems→m_rewardSelectList、m_questType→m_triggerType、
//   m_questSubType→m_triggerSubType、m_params→m_triggerData、m_eventList→m_eventDialogs、
//   m_exceptions→m_exceptionList——偏移逐一相同，2026-08-30 统一）。

// ============================================================================
// 跨类调用（全部真实类型调用；asm 桥已删除）
// ============================================================================

#include "UserQuest/reset.inc"
}

#include "UserQuest/checkAcceptableQuest.inc"
}

// ============================================================================
// UserQuest（总大小 0x761d pack(1)，布局见文件头；与 CUser.h 同步）
// ============================================================================

#pragma pack(push, 1)
#pragma pack(pop)

// ============================================================================
// 构造 / 析构 / set_parent
// ============================================================================

#include "UserQuest/set_parent.inc"
}

// ============================================================================
// reset / resetClearQuest / isClearQuest
// ============================================================================

#include "UserQuest/resetClearQuest.inc"
}

// [b28c-L9] SIGFIX:UserQuest::isClearQuest —— ORIG 0x086ab920 体为
// `call _ZNK8WongWork11CQuestClear14isClearedQuestEj; leave; ret` 直返无归一化；
// callee 权威型 unsigned char（QuestClear.h:41）⇒ caller 同型化（P9/P174）。
#include "UserQuest/isClearQuest.inc"
}

// ============================================================================
// set_quest / get_quest
// ============================================================================

#include "UserQuest/set_quest.inc"
}

#include "UserQuest/get_quest.inc"
}

// ============================================================================
// get_quest_info / get_mail_quest_info
// ============================================================================

#include "UserQuest/get_quest_info.inc"
}

#include "UserQuest/get_mail_quest_info.inc"
}
// ============================================================================
// set_authen_data
// ============================================================================

#include "UserQuest/set_authen_data.inc"
}

// ============================================================================
// accept_quest
// ============================================================================

#include "UserQuest/accept_quest.inc"
}

// ============================================================================
// giveup_quest
// ============================================================================

#include "UserQuest/giveup_quest.inc"
}

// ============================================================================
// set_trigger
// ============================================================================

#include "UserQuest/set_trigger.inc"
}

// ============================================================================
// finish_quest
// ============================================================================

#include "UserQuest/finish_quest.inc"
}

// ============================================================================
// item_reward_quest / get_rescue_questmap / check_cond_clear
// ============================================================================

// b28d-40: P223 双守卫分落——外层正向 if 包裹 + 内层 continue 复现
// ORIG 第一守卫直落循环尾、第二守卫落 nop 岛；0x086ac954，AE。
#include "UserQuest/item_reward_quest.inc"
}

// b28d-40: P223 双守卫分落（同 item_reward_quest 形）；0x086aca16，AE。
#include "UserQuest/get_rescue_questmap.inc"
}

#include "UserQuest/check_cond_clear.inc"
}

// ============================================================================
// check_quest_condition / check_quest_type_condition /
// check_quest_subtype_condition
// ============================================================================

#include "UserQuest/check_quest_condition.inc"
}

#include "UserQuest/check_quest_type_condition.inc"
}

#include "UserQuest/check_quest_subtype_condition.inc"
}

// ============================================================================
// IsDoingQuest / CheckDungeonEventStoryPause / isDoingJobChangeQuest
// ============================================================================

#include "UserQuest/IsDoingQuest.inc"
}

#include "UserQuest/CheckDungeonEventStoryPause.inc"
}

#include "UserQuest/isDoingJobChangeQuest.inc"
}

// ============================================================================
// ResetUrgentQuestWaitingList / hasEpicQuest
// ============================================================================

#include "UserQuest/ResetUrgentQuestWaitingList.inc"
}

#include "UserQuest/hasEpicQuest.inc"
}
