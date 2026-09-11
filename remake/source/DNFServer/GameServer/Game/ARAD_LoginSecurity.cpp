// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ARAD_LoginSecurity/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ARAD_LoginSecurity.h"

#include <cstring>
#include <time.h>

#include "CDataManager.h"      // G_CDataManager / PvPChannel 等级接口（W 符号）
#include "CGM_Manager.h"
#include "CSimpleSSO.h"
#include "CRijndael.h"
#include "DB_Login.h"          // DB_Login::tagDNFEventEntry 完整类型
#include "DBMgr.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "PacketBuf.h"         // G_GameWorld（与 GameWorld.h 同源声明）
#include "Stream.h"
#include "StreamDeps.h"        // G_CEnvironment

namespace {

// SIG_LOGIN_DATA +0x394c（m_pad391c 区内，ORIG `movl $0,0x394c(%eax)` 清零）的具名视图
struct SigPad394cView
{
    char m_pad[0x394c];
    int  m_field;
};

}  // namespace

#include "ARAD_LoginSecurity/CheckDailyScheduleTime.inc"
                            long lCurTime);          // GameUtils.cpp @0x0846c0a8

// 注：WongWork::CUserPremium::IsRestrictedPremium 使用 CUser.h 权威声明
// （premium_info_set 直接调用）。
namespace ARAD
{

// ============================================================================
// ARAD::PUNISH_CHECK —— 处罚检查表驱动
// ============================================================================

#include "ARAD_LoginSecurity/AradWhiteAccount.inc"


#include "ARAD_LoginSecurity/punish_check_by_HUB.inc"


#include "ARAD_LoginSecurity/punish_check_by_RES.inc"



#include "ARAD_LoginSecurity/punish_check_by_WEB.inc"


#include "ARAD_LoginSecurity/punish_check_by_SYS.inc"


#include "ARAD_LoginSecurity/punish_check_by_ARS.inc"


// ============================================================================
// ARAD::PVP_CHANNEL —— 渠道类型判定
// ============================================================================

#include "ARAD_LoginSecurity/is_integrated_pvp_channel.inc"


#include "ARAD_LoginSecurity/is_broadcast_channel.inc"


#include "ARAD_LoginSecurity/is_all_pvp_channel.inc"


#include "ARAD_LoginSecurity/is_winpoint_pvp_channel.inc"


#include "ARAD_LoginSecurity/is_common_channel.inc"


#include "ARAD_LoginSecurity/check_Login_server_group.inc"


// ============================================================================
// ARAD::pvp_grade_check ——【objdump】ORIG 0x0819ebde（无独立报告）
// bool pvp_grade_check(CUser*)
//   channelType ∉ {8,13} → 直接 true；
//   否则 level 不在 [minLevelPvP, maxLevelPvP) → trace(fmt@0x8ba8d3c)+false；
//   否则 IsAvailablePvPChannel(get_pvp_grade()) 为真 → true，
//        否则 trace(fmt@0x8ba8d7c)+false。
// ============================================================================

#include "ARAD_LoginSecurity/pvp_grade_check.inc"


// ============================================================================
// ARAD::AradWhiteAccount —— 登录白名单账号集合
// ============================================================================

#include "ARAD_LoginSecurity/dtor_AradWhiteAccount.inc"


#include "ARAD_LoginSecurity/Load.inc"

#include "ARAD_LoginSecurity/IsWhiteAccount.inc"


// ============================================================================
// ARAD::Arad_Dispatcher_Login_Impl —— 登录库分派器（code=1）
// ============================================================================

// [SplitC 2026-09-11 batch2 · TU 迁移] Arad_Dispatcher_Login_Impl 已迁出至 Arad_Dispatcher_Login_Impl.cpp（ORIG GI 键对齐）。



#include "ARAD_LoginSecurity/dtor_Arad_Dispatcher_Login_Impl.inc"


#include "ARAD_LoginSecurity/inter_dispatch.inc"


#include "ARAD_LoginSecurity/account_password_check.inc"


#include "ARAD_LoginSecurity/pad_certifiaction_check.inc"


#include "ARAD_LoginSecurity/premium_info_set.inc"


#include "ARAD_LoginSecurity/db_dispatch.inc"


#include "ARAD_LoginSecurity/punish_user_check.inc"


}  // namespace ARAD
