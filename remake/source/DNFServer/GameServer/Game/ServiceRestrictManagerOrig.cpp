// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 OnlyOrigBatch14.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN16RestrictCategory12enumToStringENS_4EnumE
//   ORIG TU     ：ServiceRestrictManager.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "DNFLexWrapper.h"        // g_dnfLex / ScanType / ScanInt / ScanStr / ScanFloat
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "LogManager.h"
#include "CEnvironment.h"         // SpinLock
#include "NpcScript.h"            // getCharacterJob / STScriptFileList
#include "CMailBoxHelper.h"       // SIG_LETTER_INFO（PrintLetter 形参）
#include "CMap.h"                 // STAICharacterMapArrangeData
#include "CDataManager.h"         // G_CDataManager()（getNeedMaterial(int)）
#include "Quest.h"                // RewardItemInfo
#include "STCharacterScript.h"    // STRDCharacterStatus / CharacterEtcStats
#include "STTileScript.h"
#include "STBreakAwayMemberData.h"
#include "NeedMaterialDataManager.h"
#include "RestrictCategory.h"
#include "CItem.h"
#include "CMission.h"             // CMission（MissionLessThan::operator()）
#include "MissionLessThan.h"
#include "STStackableScript.h"    // STStackablePackageData
#include "SyncScriptDeps.h"       // toTString
#include "CreatureScript.h"       // getActiveStatusType / getDrawLayer
#include "PetScript.h"
#include "DNFFunctionLib.h"       // timeGetTime
#include "EncryptTool.h"          // EncryptTool（G_EncryptTool）
#include "CUser.h"                // CUser（GiveItemReturnUser / DecreaseTradeLimitCountByUser）
#include "CUserCharacInfo.h"      // getCurCharacNo / getCurCharacLastPlayDungeonIndex /
#include "CInventory.h"           // CInventory::GetInvenRef
#include "CItem.h"                // CItem::make_item（已在上面 include，重复无害）
#include "Inven_Item.h"           // Inven_Item / DecreaseTradeLimitCount
#include "CDungeon.h"             // CDungeon::GetDungeonName
#include "GameEnums.h"            // ENUM_PACKETCLASS / ENUM_NOTIPACKET / ENUM_CMDPACKET
#include "W8LT_PassiveObjectScript.h"      // STPassiveObjectScript 及子对象
#include "STPassiveObjectAdditionalParticleInfo.h"
#include "CDeathTowerPlayDataStatistic.h"
#include "CacheCharacterType.h"   // ENUM_INTERNALPACKET（全局枚举，权威定义）

#include "ServiceRestrictManagerOrig/enumToString.inc"
