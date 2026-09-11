// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NewAccountLevelUpEventToJob/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "NewAccountLevelUpEventToJob/NewAccountLevelUpEventToJob.inc"


#include "NewAccountLevelUpEventToJob/dtor_NewAccountLevelUpEventToJob.inc"


#include "NewAccountLevelUpEventToJob/StartEvent.inc"


#include "NewAccountLevelUpEventToJob/StartEvent_1.inc"


#include "NewAccountLevelUpEventToJob/EndEvent.inc"


#include "NewAccountLevelUpEventToJob/StartAction.inc"


#include "NewAccountLevelUpEventToJob/IsEventing.inc"


#include "NewAccountLevelUpEventToJob/sendNewAccountLevelUpToJobData.inc"


#include "NewAccountLevelUpEventToJob/processLevelUpToJob.inc"

