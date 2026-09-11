// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFairPvPScore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "PacketGuard.h"              // CFairPvPScore.h 引用 PacketGuard&
#include "STEquipmentScript.h"        // 提供 guarded ENUM_CHARACTERJOB（DNF_ENUM_CHARACTERJOB_DEFINED）
#include "CFairPvPScore.h"

#include <string.h>

#include "GameWorld.h"                // G_GameWorld/GetChannelType
#include "MsgQueueMgr.h"              // s_msgq_mgr->put 完整类型
#include "GlobalData.h"               // s_stream_pool/s_msgq_mgr/s_systemTime_
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"       // PacketGuard 转发层 API
#include "SigTypes.h"                 // SIG_FAIR_PVP_SCORE（0x688）
#include "CItem.h"                    // CItem::make_item
#include "CInventory.h"               // insert_event_items/MakeItemPacket/getCurCharacInven*
#include "CUser.h"                    // Send/GetUID/get_charac_no/get_pvp_grade/GetServerGroup/...
#include "CUserCharacInfo.h"          // getPVPResultRefR/getCurCharFirstGrowType/get_charac_job/getCurCharacNo
#include "PvpResultType.h"            // 权威 PvpResultType（m_pvpGrade=+0x14，ORIG 084a4b94 mov 0x14(%eax)）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "ServerParameterScript.h"    // GetTraningSealGive/GetTraningFirstGive/get_pvp_item_give

extern CDataManager* G_CDataManager();

namespace fair_pvp
{

#include "CFairPvPScore/CFairPvPScore.inc"


#include "CFairPvPScore/dtor_CFairPvPScore.inc"


#include "CFairPvPScore/ResetDaily.inc"


// ---- ORIG 0x84a63f2 JobKey::operator<(const JobKey&) const -----------------
bool JobKey::operator<(const JobKey& rhs) const
{
    if (m_job < rhs.m_job)
        return true;
    // [b28 P59 块序试验] ORIG 0x84a63f2：第二比较 `cmp;jne 尾mov$0`（!= 为跳走
    // 侧、== fall-through 进第三段）；false 出口共享物理尾块（84a6433 mov$0 在
    // 三段之后）。等值嵌套形令 4.4 反向排布尝试。
    if (m_job == rhs.m_job) {
        // ORIG 0x84a63f2: 第三比较 `cmp %al,%dl; jge 尾`（jge=跳 false 尾，
        // < 落 true 岛）。字段 P211 恢复 signed char，直接比较即原有符号序。
        if (m_sub < rhs.m_sub)
            return true;
    }
    return false;
}

#include "CFairPvPScore/JobKey.inc"


#include "CFairPvPScore/_reset.inc"


#include "CFairPvPScore/_saveData.inc"


#include "CFairPvPScore/getData.inc"


#include "CFairPvPScore/UpdateScore.inc"


#include "CFairPvPScore/MakePacketSocre.inc"


#include "CFairPvPScore/SetGiveItem.inc"


#include "CFairPvPScore/GetDailyPlayCount.inc"


#include "CFairPvPScore/SetDailyPlayCount.inc"


#include "CFairPvPScore/SetRemainBonusCount.inc"


#include "CFairPvPScore/SetLastTrainingSealCount.inc"


#include "CFairPvPScore/SetLastTrainingSealBonusCount.inc"


#include "CFairPvPScore/GetRemainBonusCount.inc"


#include "CFairPvPScore/GetLastTrainingSealCount.inc"


#include "CFairPvPScore/GetLastTrainingSealBonusCount.inc"


// b28d-21：删除**发明符号** `_ZN8fair_pvp12_GLOBAL__N_124IsFairPvpTrainingChannelEv`
// （ORIG nm 无此符号；P49：-O0 下 static inline helper 仍产真实 call 符号 = 既 DIFF
// 又造 missing）。ORIG 0x84a48f8 loadData 区间实测 4 次 `call G_GameWorld` +
// 4 次 `call GetChannelType`（0x84a4b??/0x84a4c?? 段）⇒ 两个使用点各直接写两次
// `G_GameWorld()->GetChannelType()`（P183 禁指针缓存，每点重发全链）。

#include "CFairPvPScore/loadData.inc"


}  // namespace fair_pvp
