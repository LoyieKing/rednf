#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserDepsB/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <stdlib.h>

#include "CUser.h"                    // CUser/cUserHistoryLog/m_checkSumSeed/
                                      // m_historyLog/m_codeHackCheck/getCharacLevelUpGift_AccountOnce
#include "CUserCharacInfo.h"          // getCurCharacR/getPVPResultR/getCurCharacInvenR/
                                      // getCurCharacSkillR/getCurCharacNo/get_charac_job/
                                      // get_charac_level/getCurCharFirstGrowType/PvpResultType
#include "Quest.h"                    // Quest 完整类型（m_mailTitle +0x10c /
                                      // m_mailContents +0x110 / m_mailItems +0x114）
#include "Inven_Item.h"               // Inven_Item（m_field1/m_addInfo/m_addInfo2/m_endurance）
#include "CInventory.h"               // makeCheckSum(int,int) const / OS_API::snprintf
#include "SkillSlot.h"                // SkillSlot::makeCheckSum(int) const
#include "CDataManager.h"             // G_CDataManager/find_item/m_levelUpReward/getItemList
#include "DataManagerInitDepends.h"   // CLevelRewardTable/CLevelRewardTableMgr
#include "CGuildServerProxy.h"        // CEventManager/CEventBase::IsEventing/DeliveryMsg/
                                      // BuyItemDeliveryMsg
#include "NewAccountLevelUpEventToJob.h"  // processLevelUpEvent 调用的完整类
#include "GlobalData.h"               // s_event_manager/s_stream_pool/s_msgq_mgr/s_GM_Manager
#include "LevelupSupportEventManger.h"
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CItemList.h"                // create_item/ENUM_ITEM_CREATE_TYPE
#include "CItem.h"                    // CItem::make_item/get_index
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf clear/put_*/finalize）
#include "CStreamGuard.h"             // CStreamGuard（operator*/operator<<）
#include "PacketTypes.h"              // MSG_MAILBOX_SEND 完整定义（sizeof 0x145，
                                      // m_name11@+0x11/m_itemId39@+0x39/m_extra3d@+0x3d）
#include "MsgQueueMgr.h"              // MsgQueueMgr::put/DB_Q
#include "DB_SaveEventLevelupSupport.h"
#include "DB_EventAccountCharacterLevelUpOnceGift.h"
#include "GameWorld.h"                // G_GameWorld/IsIntegratedPvPBaseChannel
#include "CEnvironment.h"             // G_CEnvironment/m_serverEnvir.m_serverGroup
#include "CGM_Manager.h"              // CGM_Manager::GetCurrentGmMode
#include "CParty.h"                   // CParty::Reset_party_overlapped_drop_ratio
#include "CExpandEquipslot.h"         // CExpandEquipslot::Change_Equip
#include "HeroMissionEvent.h"         // HeroMissionEvent::processMission/MissionNo

#include "CUserDepsB/change_equip_expand.inc"


#include "CUserDepsB/eventDeliveryBuyItem.inc"


#include "CUserDepsB/_processLevelReward.inc"


#include "CUserDepsB/_postQuestTriggerItem.inc"


#include "CUserDepsB/processLevelUpEventReward.inc"


#include "CUserDepsB/processLevelUpEvent.inc"


#include "CUserDepsB/getCurCharacInvenCheckSum.inc"


#include "CUserDepsB/getCurCharacSkillCheckSum.inc"


// ============================================================================
// 依赖叶子（ORIG 直调；地址与语义见文件头清单）
// ============================================================================

#include "CUserDepsB/GetServerGroupToPvP.inc"


// DeliveryMsg::DeliveryMsg（ORIG W 0x086978c0，四字段清零）定义在
// CtorProducers_1.cpp（NL4 unit-at-a-time 塌缩机制：同 TU 可见 callee 使
// BuyItemDeliveryMsgC1 outgoing 帧塌缩 sub $0x4，ORIG sub $0x18）。

#include "CUserDepsB/setBaseData.inc"


#include "CUserDepsB/BuyItemDeliveryMsg.inc"


#include "CUserDepsB/LevelUpRewardItemTableKey.inc"


#include "CUserDepsB/get_data.inc"


#include "CUserDepsB/get_data_when_growup.inc"


// ---- CLevelRewardTable 访问器（偏移与权威布局逐指令核对）----

#include "CUserDepsB/GetRewardItemNo.inc"


#include "CUserDepsB/GetItemCount.inc"


#include "CUserDepsB/GetMailTitle.inc"


#include "CUserDepsB/GetMailString.inc"


#include "CUserDepsB/GetJobAffection.inc"


#include "CUserDepsB/IsOn.inc"


#include "CUserDepsB/IsChallengingJob.inc"


#include "CUserDepsB/GetProperLevelRewardTableR.inc"


#include "CUserDepsB/GetProperLevelRewardTableR_1.inc"


#include "CUserDepsB/IsHaveStartLevelReward.inc"


#include "CUserDepsB/IsOnServer.inc"


// ---- LevelupSupport 域 ----

#include "CUserDepsB/CheckNewStep.inc"


#include "CUserDepsB/UpdateEventInfo.inc"


#include "CUserDepsB/SendCharacState.inc"


#include "CUserDepsB/Process.inc"


// ---- DB 请求封装 ----

#include "CUserDepsB/makeRequest.inc"


#include "CUserDepsB/makeRequest_1.inc"

