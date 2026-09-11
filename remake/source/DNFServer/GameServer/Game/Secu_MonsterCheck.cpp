// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 UserCoreDepsB2.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN17Secu_MonsterCheck10SetUserPtrEP5CUser
//   ORIG TU     ：Secu_MonsterCheck.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "CUser.h"                    // CUser/_Charac_info/Secu_HackLogCheck/
#include "CUserCharacInfo.h"          // getCurCharacR/getTagCharacR/getTagCharacSkillW/
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "AvatarRechargeServer.h"     // CalcDurabilityChangeEquip/CalcDurabilityDetail 声明
#include "CInventory.h"               // MakeEquipList/GetInvenSlot/insertItemIntoInventory/
#include "CExpandEquipslot.h"         // GetCharacExpandDataR(9) 返回类型与 GetInvenSlot
#include "CEquipItem.h"               // CEquipItem 视图（GetUsableEquipmentType/m_field1bc）
#include "CItem.h"                    // is_stackable/get_index/getUsableLevel/GetItemType
#include "CDataManager.h"             // G_CDataManager/find_item/m_serverParameter(+0x68)
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "CCharacter.h"               // _Additioal_info（GetAdditionalInfo 返回值）
#include "CBattle_Field.h"            // check_grid_clear（CheckMapClear 转发目标）
#include "CDungeon.h"                 // isNoFatigueDungeon/m_villageAttackEventFlag/m_bloodDungeonType
#include "CParty.h"                   // 路由 API/IsUseFatigueDungeon/CheckMapClear/
#include "CSpecialItemRoutingManager.h"
#include "CBossTower.h"               // WongWork::CBossTower（getBossTower 返回值）
#include "WarField.h"                 // MapInfo::m_itemMap（+0xa18 = CBossStage+0x9f4+0x24）
#include "CGenUniqueNo.h"             // WongWork::CGenUniqueNo::genIPGNo
#include "GameRand.h"                 // get_rand_int
#include "CStatisticServerProxy.h"    // statistc_proxy::add
#include "CSystemTime.h"              // getCurDate
#include "GlobalData.h"               // s_systemTime_
#include "LogManager.h"               // logFormat

#include "Secu_MonsterCheck/SetUserPtr.inc"
