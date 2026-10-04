// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TaiwanDBDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— TaiwanDBDispatcher（台湾 DB 分发器，ORIG T @0x0815a134 族）。
// 报告：docs/class_func_reports/TaiwanDBDispatcher/（6 函数齐全）。
// 依赖：init 中 new 的 15 个子类（LoadServiceRestrictPolicy 等）为 ORIG W ctor；
// 子类 ctor/dtor 最小实现（仅基类 ctor + vptr，证据：global_constructors_
// keyed_to_TaiwanDBDispatcher 报告各 ctor 区 movl code + call 基类 C1 + 设子类 vtable；
// throw() 使 ORIG new 链无 EH 块）。
#include "TaiwanDBDispatcher.h"

#include <algorithm>

#include "TaiwanInterfaceDBDispatcher.h"

#include "LoadServiceRestrictPolicy.h"
#include "SaveServiceRestrictPolicy.h"
#include "LoadGeolocation.h"
#include "CheckStayTimeEvent.h"
#include "ResetStayTimeEvent.h"
#include "DBUpdateExitTime.h"
#include "DBSelectCreateDnfEventInfo.h"
#include "DBSelectCreateDnfUserInfo.h"
#include "DBInsertCreateDnfUserInfo.h"
#include "DBSelectMobileAuthRewardTw.h"
#include "DBSelectPcroomDailyReward.h"
#include "DBUpdatePcroomDailyReward.h"
#include "DBInsertGiveBoxEventReward.h"
#include "DBLoadHeroMissionEventData.h"
#include "DBSaveHeroMissionEventData.h"

#include "Stream.h"          // Stream::GetOutBuffer（DBUpdateExitTime::dispatch）
#include "TaiwanQueries.h"   // Taiwan::SigUpdateExitTime
#include "GlobalData.h"      // GlobalData::s_psimpleSSO
#include "CSimpleSSO.h"      // CSimpleSSO::updateSSOExpireTime
#include "LogManager.h"      // LogManager::logFormat
#include "TaiwanInternal.h"   // Taiwan::internal_stream
#include "MySQL.h"            // MySQL::{set_query,exec,get_n_rows,fetch,get_int,get_byte,get_binary,blob_to_str}
#include "DBMgr.h"            // DBMgr::GetDBHandle
#include "MsgQueueMgr.h"      // MsgQueueMgr::DB_Q / put
#include "CEnvironment.h"     // G_CEnvironment / get_server_group
#include "CSystemTime.h"      // CSystemTime::getCurSec
#include "GameTypes.h"        // cMyTrace
#include "InterSelectCreateDnfEventInfo.h"
#include "InterSelectCreateDnfUserInfo.h"
#include "InterSelectMobileAuthReward.h"
#include "InterSelectPcroomDailyReward.h"
#include "CUser.h"            // CUser::GetUID / get_acc_id / m_rewarded
#include "CGuildServerProxy.h"  // CEventManager/CEventBase（GetRepeatEvent/IsEventing）
// ORIG T 0x081090b4（u32 版；权威声明见 CInventory.h:535，本 TU 同款局部声明）
char* NumberToString(unsigned int value, int radix);
// ORIG T 0x0846c0a8（定义 GameUtils.cpp:88）
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);

// ---- TaiwanDBDispatcher ----

#include "TaiwanDBDispatcher/TaiwanDBDispatcher.inc"

// ---- DB* 子类最小实现（仅基类 ctor + vptr；业务方法不在本批）----

#include "TaiwanDBDispatcher/LoadServiceRestrictPolicy.inc"
#include "TaiwanDBDispatcher/dtor_LoadServiceRestrictPolicy.inc"

#include "TaiwanDBDispatcher/SaveServiceRestrictPolicy.inc"
#include "TaiwanDBDispatcher/dtor_SaveServiceRestrictPolicy.inc"

#include "TaiwanDBDispatcher/LoadGeolocation.inc"
#include "TaiwanDBDispatcher/dtor_LoadGeolocation.inc"

#include "TaiwanDBDispatcher/CheckStayTimeEvent.inc"
#include "TaiwanDBDispatcher/dtor_CheckStayTimeEvent.inc"

#include "TaiwanDBDispatcher/ResetStayTimeEvent.inc"
#include "TaiwanDBDispatcher/dtor_ResetStayTimeEvent.inc"

#include "TaiwanDBDispatcher/DBUpdateExitTime.inc"
#include "TaiwanDBDispatcher/dtor_DBUpdateExitTime.inc"


// SigUpdateExitTime（TaiwanQueries.h 私有 m_timeA/m_timeB）偏移视图：
// ORIG dispatch 0x0815b97c 直接读 +0x00/+0x04 两 uint。
#pragma pack(push, 1)
struct DBUpdateExitTimeView
{
    unsigned int m_timeA;   // +0x00
    unsigned int m_timeB;   // +0x04
};
#pragma pack(pop)

#include "TaiwanDBDispatcher/dispatch.inc"

#include "TaiwanDBDispatcher/DBSelectCreateDnfEventInfo.inc"
#include "TaiwanDBDispatcher/dtor_DBSelectCreateDnfEventInfo.inc"

#include "TaiwanDBDispatcher/DBSelectCreateDnfUserInfo.inc"
#include "TaiwanDBDispatcher/dtor_DBSelectCreateDnfUserInfo.inc"

#include "TaiwanDBDispatcher/DBInsertCreateDnfUserInfo.inc"
#include "TaiwanDBDispatcher/dtor_DBInsertCreateDnfUserInfo.inc"

#include "TaiwanDBDispatcher/DBSelectMobileAuthRewardTw.inc"
#include "TaiwanDBDispatcher/dtor_DBSelectMobileAuthRewardTw.inc"

#include "TaiwanDBDispatcher/DBSelectPcroomDailyReward.inc"
#include "TaiwanDBDispatcher/dtor_DBSelectPcroomDailyReward.inc"

#include "TaiwanDBDispatcher/DBUpdatePcroomDailyReward.inc"
#include "TaiwanDBDispatcher/dtor_DBUpdatePcroomDailyReward.inc"

#include "TaiwanDBDispatcher/makeRequest.inc"

#include "TaiwanDBDispatcher/DBInsertGiveBoxEventReward.inc"
#include "TaiwanDBDispatcher/dtor_DBInsertGiveBoxEventReward.inc"

#include "TaiwanDBDispatcher/DBLoadHeroMissionEventData.inc"
#include "TaiwanDBDispatcher/dtor_DBLoadHeroMissionEventData.inc"

#include "TaiwanDBDispatcher/DBSaveHeroMissionEventData.inc"
#include "TaiwanDBDispatcher/dtor_DBSaveHeroMissionEventData.inc"

// ---- TaiwanInterfaceDBDispatcher 基类（ORIG W：C1 0x0815df66 / D1 0x0815df7e / D0 0x0815dfa4 / getCode 0x0815dfc2）----

#include "TaiwanDBDispatcher/TaiwanInterfaceDBDispatcher.inc"
#include "TaiwanDBDispatcher/dtor_TaiwanInterfaceDBDispatcher.inc"
#include "TaiwanDBDispatcher/getCode.inc"

#include "TaiwanDBDispatcher/dtor_TaiwanDBDispatcher.inc"

#include "TaiwanDBDispatcher/operator.inc"

#include "TaiwanDBDispatcher/getDispatcher.inc"

#include "TaiwanDBDispatcher/addDispatcher.inc"

#include "TaiwanDBDispatcher/delDispatcher.inc"

#include "TaiwanDBDispatcher/init.inc"

// ============================================================================
// OnlyOrigBatch8 —— Taiwan DB 分发器业务方法（ORIG 0x0815b362..0x0815db96 族）。
// 报告：docs/class_func_reports/{CheckStayTimeEvent,DBInsertCreateDnfUserInfo,
// DBSelectCreateDnfUserInfo,DBSelectMobileAuthRewardTw,DBSelectPcroomDailyReward,
// DBUpdatePcroomDailyReward,DBInsertGiveBoxEventReward,DBLoadHeroMissionEventData,
// DBSaveHeroMissionEventData,DBUpdateExitTime,DBSelectCreateDnfEventInfo}。
// 异常均为 throw const char*（_ZTIPKc @0x936b288），catch 记 logFormat 返 0。
// ============================================================================

// SigLoadHeroMissionData/SigSaveHeroMissionData（TaiwanQueries.h 私有成员）视图。
struct SigHeroMissionDataView
{
    unsigned int m_field0;        // +0x00
    unsigned char m_data[0x100];  // +0x04
};

#include "TaiwanDBDispatcher/dispatch_1.inc"

#include "TaiwanDBDispatcher/sendStayTimeData.inc"

#include "TaiwanDBDispatcher/makeRequest_1.inc"

#include "TaiwanDBDispatcher/dispatch_2.inc"

#include "TaiwanDBDispatcher/dispatch_3.inc"

#include "TaiwanDBDispatcher/makeRequest_2.inc"

#include "TaiwanDBDispatcher/dispatch_4.inc"

#include "TaiwanDBDispatcher/makeRequest_3.inc"

#include "TaiwanDBDispatcher/dispatch_5.inc"

#include "TaiwanDBDispatcher/dispatch_6.inc"

#include "TaiwanDBDispatcher/dispatch_7.inc"

#include "TaiwanDBDispatcher/dispatch_8.inc"

#include "TaiwanDBDispatcher/dispatch_9.inc"

#include "TaiwanDBDispatcher/dispatch_10.inc"
// ---- OnlyOrigBatch10：Taiwan DB 域 dispatch 实现 ----

#include "TaiwanDBDispatcher/dispatch_11.inc"

#include "TaiwanDBDispatcher/sendCountryData.inc"

#include "TaiwanDBDispatcher/dispatch_12.inc"

#include "TaiwanDBDispatcher/sendRestrictData.inc"

#include "TaiwanDBDispatcher/dispatch_13.inc"

#include "TaiwanDBDispatcher/dispatch_14.inc"
