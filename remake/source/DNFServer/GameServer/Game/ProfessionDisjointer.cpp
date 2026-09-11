// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 ExpertJobLink4.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN10expert_job16CDisjointMachineC2EP5CUserPKcicii
//   ORIG TU     ：ProfessionDisjointer.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "CExpertJob.h"
#include <math.h>
#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript(int) /
#include "CExpertJobList.h"    // STExpertJobEtcScript
#include "CItem.h"             // GetSellPrice/get_rarity/get_index/get_grade/
#include "CItemAmplifier.h"    // stAmplifyOption_t::isIdentified
#include "CPrivateStoreMgr.h"  // private_store::{GetInstancePrivateStoreMgr,
#include "CUser.h"
#include "DisJoint.h"          // DisJointResult
#include "CStackableItem.h"       // m_int324(+0x324) 拆解券等级门槛
#include "DisPatcherCore.h"       // DisPatcher_DisJointItem（静态成员定义）
#include "GameRand.h"          // get_rand_int
#include "GameWorld.h"         // G_GameWorld 及各查询/移动接口
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // GlobalData::s_villageObjectMgr
#include "Inven_Item.h"        // Inven_Item::m_amp
#include "PacketGuard.h"
#include "Arad_DataManager.h"      // ARAD::Singleton<T>
#include "ServiceRestrictManager.h"

namespace expert_job
{
#include "ProfessionDisjointer/CDisjointMachine.inc"
}  // namespace expert_job
