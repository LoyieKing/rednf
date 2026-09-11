// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CExpertJob/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ expert_job 核心类型实现（地址证据见 CExpertJob.h 头注释）。
#include "CExpertJob.h"

#include "CDataManager.h"
#include "CExpertJobList.h"
#include "CInventory.h"
#include "CItem.h"
#include "CGuildServerProxy.h"   // WongWork::CGMAccounts::isGM
#include "CStreamGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include <cstring>

#include "GameRand.h"
#include "SigTypes.h"
#include "SkillSlot.h"

#include "CExpertJob/_reset.inc"
{
    m_giveupCount = 0;
    m_endurance = 0;
    m_enchantCount = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
}

namespace collision_detection
{

#include "CExpertJob/Rectangle.inc"

#include "CExpertJob/include.inc"


}  // namespace collision_detection

namespace expert_job
{

#include "CExpertJob/ExpertJobMaterial.inc"

#include "CExpertJob/Reset.inc"


#include "CExpertJob/CExpertJob.inc"

#include "CExpertJob/dtor_CExpertJob.inc"

#include "CExpertJob/GetType.inc"

#include "CExpertJob/SetType.inc"

#include "CExpertJob/CDisjointMachine.inc"


#include "CExpertJob/dtor_CDisjointMachine.inc"

#include "CExpertJob/reset.inc"


#include "CExpertJob/put_object_info.inc"


#include "CExpertJob/put_disjoint_machine_info.inc"


#include "CExpertJob/CDisjointer.inc"

#include "CExpertJob/dtor_CDisjointer.inc"

#include "CExpertJob/CEnchanter.inc"

#include "CExpertJob/dtor_CEnchanter.inc"

// ============================================================================
// CExpertJob 静态工具（ORIG，见 expert_job__CExpertJob/*.md）
// ============================================================================

#include "CExpertJob/GetSuccessRate.inc"


#include "CExpertJob/IsSuccess.inc"


#include "CExpertJob/IsLearnRecipe.inc"


// [SplitC 2026-09-11 batch2 · TU 迁移] IncreaseExpertJobExp 已迁出至 ExpertJob.cpp（ORIG GI 键对齐）。



// ============================================================================
// CCharacExpertJob（ORIG，见 expert_job__CCharacExpertJob/*.md）
// ============================================================================

#include "CExpertJob/CCharacExpertJob.inc"


#include "CExpertJob/dtor_CCharacExpertJob.inc"


#include "CExpertJob/InitScript.inc"


#include "CExpertJob/get_recipe.inc"


#include "CExpertJob/set_recipe.inc"



#include "CExpertJob/learnRecipe.inc"


#include "CExpertJob/loadData.inc"


#include "CExpertJob/_saveData.inc"


#include "CExpertJob/set_giveup_count.inc"


#include "CExpertJob/get_giveup_count.inc"

}  // namespace expert_job
