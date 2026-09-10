// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NewAccountLevelUpEventToJob/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— NewAccountLevelUpEventToJob（新账号升级转职奖励事件）。
// 全部函数逐指令对照 docs/class_func_reports/NewAccountLevelUpEventToJob/*.md
// 的 ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   C1 0x081b8170 / D1 0x081b818c / D0 0x081b81bc / StartEvent() 0x081b81da /
//   StartEvent(Word_Param) 0x081b81f6 / EndEvent() 0x081b824c /
//   StartAction() 0x081b8246 / IsEventing 0x081b8816（objdump 转写）/
//   processLevelUpToJob 0x081b82a4 / sendNewAccountLevelUpToJobData 0x081b829e
#include "NewAccountLevelUpEventToJob.h"

#include <string.h>

#include "CUser.h"                    // CUser::getCharacLevelUpGift_AccountOnce
#include "CUserCharacInfo.h"          // getCurCharFirstGrowType/get_charac_job/
                                      // get_charac_level/getCurCharacNo
#include "CDataManager.h"             // G_CDataManager/m_levelUpReward/getItemList
#include "Inven_Item.h"               // Inven_Item（m_addInfo）
#include "CItemList.h"                // create_item/ENUM_ITEM_CREATE_TYPE
#include "CInventory.h"               // OS_API::snprintf
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "DB_EventAccountCharacterLevelUpOnceGift.h"
#include "GameTypes.h"                // cMyTrace/LogManager
#include "GameWorld.h"                // G_GameWorld/IsIntegratedPvPBaseChannel
#include "CEnvironment.h"             // G_CEnvironment/m_serverEnvir.m_serverGroup

// ORIG C1 0x081b8170：CEventBase::CEventBase() + vptr 设置，无自身字段。
#include "NewAccountLevelUpEventToJob/NewAccountLevelUpEventToJob.inc"
}

// ORIG D1 0x081b818c：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查。
#include "NewAccountLevelUpEventToJob/dtor_NewAccountLevelUpEventToJob.inc"
}

// ORIG 0x081b81da：仅 SetEventFlag(true)，无日志。
#include "NewAccountLevelUpEventToJob/StartEvent.inc"
}

// ORIG 0x081b81f6：虚调 StartEvent()（vptr+8）后
// cMyTrace(__PRETTY_FUNCTION__,0x18,0)("NewAccountLevelUpEvent Event Start!")。
#include "NewAccountLevelUpEventToJob/StartEvent_1.inc"
}

// ORIG 0x081b824c：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x2b,0)。
#include "NewAccountLevelUpEventToJob/EndEvent.inc"
}

// ORIG 0x081b8246：空函数（push ebp/mov esp,ebp/pop ebp/ret）。
#include "NewAccountLevelUpEventToJob/StartAction.inc"
}

// ORIG 0x081b8816：直调 CEventBase::IsEventing(this, user)（vtable+0x3c 覆盖）。
#include "NewAccountLevelUpEventToJob/IsEventing.inc"
}

// ORIG 0x081b829e：空函数（push ebp/mov esp,ebp/pop ebp/ret）。
#include "NewAccountLevelUpEventToJob/sendNewAccountLevelUpToJobData.inc"
}

// ORIG T 0x081b82a4 sz 0x515。
// 新账号升级转职奖励：查奖励表（growup 聚合 / 单项）→ 账号级 once-gift 多重
// 映射去重（同 job 区间内已存在同 level 则中止）→ 插入 (level, job) → DB
// 上报 → 逐条 create_item + 邮寄（标题 game_server_msg_285，正文
// snprintf(game_server_msg_286, level)），失败走 logFormat。成功返回 1，
// 前置失败返回 0。
// b28d-31: N 封顶（N82/P197）——ORIG 0x081b82a4；351 vs 353（帧/槽）。
#include "NewAccountLevelUpEventToJob/processLevelUpToJob.inc"
}
