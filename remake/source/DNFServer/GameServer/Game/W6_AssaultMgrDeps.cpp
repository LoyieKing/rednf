#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_AssaultMgrDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ pvp_assault::CAssaultMgr Wave6 补批实现（WAMgr：only_orig 23 符号清零）。
// 转写来源：docs/class_func_reports/pvp_assault__CAssaultMgr/ 各报告（## 汇编 为准，
// 每个定义前标注 ORIG T/W 地址与尺寸；ORIG 内联浮点常量 _DAT_08c1d18c = -100.0f）。
//
// 本 TU 同时承载 OnRequestAssault / check_party_teleport_user 链上的独立依赖定义：
//   IsStayGate(CUser*)                                ORIG T 0x82edc05 (0x81)
//   CPowerWarLog 生命周期/统计小方法族                  ctor T 0x848021e / Reset T 0x8480232 /
//                                                     Inc* W 见 CPowerManager.h 字段注释
//   CPowerManager::GetPowerWarLog()                   ORIG W 0x82f0906
#include "CAssaultMgr.h"

#include <map>
#include <cmath>

#include <utility>

#include "MSG_DIE_ASSAULT_PLAYER.h"
#include "GameTypes.h"            // ENUM_ERROR
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "CInventory.h"
#include "CEquipItem.h"
#include "Inven_Item.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_power_manager/s_event_manager/s_villageMonsterMgr/s_systemTime_
#include "CPartyTelePort.h"      // CParty::GetPartyTelePort 返回类型
#include "CSystemTime.h"          // CSystemTime::getCurSec
#include "CDataManager.h"         // G_CDataManager（m_serverParameter@+0x68）
#include "CPowerManager.h"
#include "CGameManager.h"         // G_CGameManager()->GetParty(int)
#include "CEnvironment.h"        // G_CEnvironment()->m_serverEnvir.m_serverGroup(+0x378)
#include "GameWorld.h"            // G_GameWorld()/getUserVillage()/IsPowerWarSafeZone()
#include "Village.h"              // Village::get_gate_area
#include "CVillageMonsterMgr.h"   // village_attacked::OnEvent
#include "CPrivateStoreMgr.h"     // private_store 单例链
#include "CExpertJob.h"           // expert_job::CAlchemist::GetAssaultPrevent
#include "TimerDispatcher.h"      // TimerCompleteLoadAssaultTimeout::registNextTimer
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent / CEventBase::IsEventing
#include "LogManager.h"

#include "W6_AssaultMgrDeps/IsStayGate.inc"

namespace pvp_assault
{

namespace
{

// ---------------------------------------------------------------------------
// ORIG 0x0822f6d2：内联 getAssaultPlace(CUser) 的公共前奏 —— 读用户挂接场次 id。
// （函数体内联消失，仅保留语义形状。）
// ---------------------------------------------------------------------------
inline int userIdPlaceId(CUser* user)
{
    return user->GetAssaultPlace();
}

// expert_job::CAlchemist::GetAssaultPrevent 为 static（无 this 槽，
// CExpertJob.h:272 权威声明，ORIG T 0x085d137c）。§4.5 正解：直接静态调用
// `CAlchemist::GetAssaultPrevent(user, flag)`，无需成员函数指针 ABI 转接。
// （DisPatcherCore.cpp 已同型直呼。）

// PowerWarOn：ORIG 各调用点均为 3 条内联形（s_power_manager 装载+call
// IsPowerWarEventOn），无独立符号；P49 —— -O0 下 inline 函数仍发真 call，
// 必须宏展开（[b23S3C]）。
#define PowerWarOn() (GlobalData::s_power_manager->IsPowerWarEventOn())

// CDataManager 内尚未具名化的三个 int 标量（+0x600/+0x604/+0x608，
// importBloodDungeon 区；字段化归 CDataManager 域批次，本批按具名视图读取）。
// 视图成员访问让 4.4 折叠单条 disp（ORIG `mov 0x604(%eax),%eax`）；inline helper
// 在 -O0 发真 call（P49）、裸 char* 偏移多发 add，均不可用。
struct CDataManagerTailView
{
    unsigned char m_reserved[0x600];
    int m_field600;
    int m_field604;
    float m_field608;                                   // ORIG `flds 0x608(%eax)`
};
#define DM_FIELD(n) \
    (reinterpret_cast<const CDataManagerTailView*>(G_CDataManager())->m_field##n)

}  // unnamed namespace

#include "W6_AssaultMgrDeps/Reset.inc"


#include "W6_AssaultMgrDeps/_GetNextAssaultPlaceId.inc"


#include "W6_AssaultMgrDeps/_GetAssaultPlace.inc"


#include "W6_AssaultMgrDeps/FrocedCloseAssaults.inc"


#include "W6_AssaultMgrDeps/OnChangeHp.inc"


#include "W6_AssaultMgrDeps/OnAbuseUser.inc"


#include "W6_AssaultMgrDeps/OnConnectP2P.inc"


#include "W6_AssaultMgrDeps/OnRevivalUser.inc"


#include "W6_AssaultMgrDeps/OnCompleteLoad.inc"


#include "W6_AssaultMgrDeps/OnDieUser.inc"


#include "W6_AssaultMgrDeps/AvailableAssault.inc"


#include "W6_AssaultMgrDeps/OnEvadeAssaultPlace.inc"


#include "W6_AssaultMgrDeps/OnTimeoutConnectP2P.inc"


#include "W6_AssaultMgrDeps/OnTimeoutEndAssault.inc"


#include "W6_AssaultMgrDeps/OnTimeoutCloseAssault.inc"


#include "W6_AssaultMgrDeps/OnTimeoutCompleteLoad.inc"


#include "W6_AssaultMgrDeps/OnConnectP2PAfterAssault.inc"


#include "W6_AssaultMgrDeps/OnTimeoutConnectP2PAfterAssault.inc"


#include "W6_AssaultMgrDeps/OnTimeoutCompleteLoadAfterAssault.inc"


#include "W6_AssaultMgrDeps/_SendVictoryInfo.inc"


#include "W6_AssaultMgrDeps/check_party_teleport_user.inc"


// ===========================================================================
// GetRequestAssaultNeedMoney —— ORIG T 0x082edc86 (0x333)
// 价格表 + chaos 时效折算 + 混沌点权重 + 等级差系数（浮点公式逐指令转写，
// ROUND 采用 ORIG fstcw(0xc)/fistpl 截断形态，等价 lrintf）。
// ===========================================================================
// ORIG 的浮点→整型截断序列（fnstcw/fldcw RC=0x0c/fistpl）由 4.4 -O0 的
// `static_cast<int>(float)` 直接产出；lrintf 形会发真 call。
#define ROUND_NEAREST_EVEN(v) (static_cast<int>(v))

#include "W6_AssaultMgrDeps/GetRequestAssaultNeedMoney.inc"


#include "W6_AssaultMgrDeps/OnRequestAssault.inc"


}  // namespace pvp_assault

#include "W6_AssaultMgrDeps/IsStayGate_1.inc"


#include "W6_AssaultMgrDeps/CPowerWarLog.inc"


#include "W6_AssaultMgrDeps/Reset_1.inc"


#include "W6_AssaultMgrDeps/IncAssaultAttemptCount.inc"

#include "W6_AssaultMgrDeps/IncAssaultAttemptFailureCount.inc"

#include "W6_AssaultMgrDeps/IncAssaultAttemptSuccessCount.inc"

#include "W6_AssaultMgrDeps/IncLoadingTimeOutCountSecede.inc"

#include "W6_AssaultMgrDeps/IncP2PTimeOutCountSecede.inc"

#include "W6_AssaultMgrDeps/IncP2PBadCountFailure.inc"

#include "W6_AssaultMgrDeps/IncP2PBadCountSecede.inc"

#include "W6_AssaultMgrDeps/IncStartSuccessCount.inc"

#include "W6_AssaultMgrDeps/IncStartFailureCount.inc"

#include "W6_AssaultMgrDeps/IncTotalJoinAssaultUser.inc"

#include "W6_AssaultMgrDeps/IncTotalLeaveAssaultUser.inc"


#include "W6_AssaultMgrDeps/GetPowerWarLog.inc"


#include "W6_AssaultMgrDeps/PrintPowerWarAssultReport.inc"

