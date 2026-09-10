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
#include "CUser.h"            // CUser::GetUID / get_acc_id / m_field8ec31
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

// ORIG 0x0815b97c：GetOutBuffer<Taiwan::SigUpdateExitTime> null → throw "handle
// null"；updateSSOExpireTime(m_timeA, m_timeB) != 1 → throw "exec"；catch
// (const char*) 记 logFormat(1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp",
// 0x1d4, __PRETTY_FUNCTION__, "[Taiwan, Auth] update member_login last_play_time.
// (error:%s)", e) 返 0；成功返 1。
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

// ORIG T 0x0815cf06（报告 DBUpdatePcroomDailyReward/makeRequest.md）：PC 房日奖励
// 落库请求：internal_stream(0xe, uid) → MSG{m_id=acc_id, m_remain=user+0x8ec32}。
// [b28d-lane6] AE：原 NEAR 残差 = cMyTrace 具名对象槽向反（ORIG guard@-0x24 /
// trace@-0x1c，4.4 具名形 guard@-0x14 / trace@-0x24）。改 P221 全表达式临时链
// `cMyTrace(...)(...)` 后槽向与 ORIG 一致（75=75 逐条全等，仅余 nop 填充）。
// 旧 b22「N82 槽号不可翻」判词作废。
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

// ORIG 0x0815b362：GetOutBuffer<Taiwan::SigStayTimeEvent> null → throw
// "context null"；GetDBHandle(9,0) null → throw "handle null"；
// set_query("seLect reward_flag from event_stay_time_charac where m_id=%s",
// NumberToString(m_accId))；exec 失败 → throw "select_exec"；
// get_n_rows()==0 → 插行("inSert into event_stay_time_charac values('%s', 1, 1,
// now())") exec 失败 throw "insert_exec" 后 sendStayTimeData(uid)；否则
// fetch 失败 throw "fetch"；get_int(0,&flag) 失败 throw "get_int"；
// flag!=0 直接返 1；否则 UPDATE 后 sendStayTimeData(uid)。成功返 1。
#include "TaiwanDBDispatcher/dispatch_1.inc"

// ORIG 0x0815b6ac：Acquire("localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x192)
// → CStreamGuard(true) → internal_stream(guard, 0x5, uid) →
// GetInBuffer<Taiwan::SigStayTimeEvent> 非空时 MsgQueueMgr::put(DB_Q, guard)。
#include "TaiwanDBDispatcher/sendStayTimeData.inc"

// ORIG 0x0815b89a（static）：Acquire(file, 0x1b9) → CStreamGuard(true) →
// internal_stream(guard, 0x8, a) → GetInBuffer<Taiwan::SigUpdateExitTime> 非空
// 时 init()、internal_stream 第三参传 b（ORIG 0xc(%ebp) 实证）、set(a, c)、
// MsgQueueMgr::put(DB_Q, guard)。
#include "TaiwanDBDispatcher/makeRequest_1.inc"

// ORIG 0x0815bb8c：GetDBHandle(9,0) null → throw "handle null"；
// set_query("seLect rate, HOUR(CURTIME()), event_day FROM event_create_dnf_info
// WHERE occ_time=DATE_ADD(CURDATE(), INTERVAL HOUR(CURTIME()) HOUR)")；exec 失败
// throw "select_exec"；get_n_rows()==0 throw "get_n_rows=0"；fetch 失败 throw
// "fetch"；get_int(0/1/2) 失败 throw "get_int N"；
// InterSelectCreateDnfEventInfo::makeRequest(rate,hour,eventDay)；
// cMyTrace(0x20d)("[Taiwan, Event] select event_create_dnf_info. hour:%d rate:%d
// event_day:%d", c, a, b)；catch：getCurSec→localtime_r→makeRequest(-1,
// tm_hour, 0)，logFormat(0x219) 返 0。成功返 1。
// [b28d-solo-TaiwanDBDispatcher] AE（194/194）：原 NEAR 残差「trace 实参 callee-saved
// 对调」真因 = 实参顺序写错（真 bug）——ORIG 0x0815be02 push 序 0x8=hour/0xc=rate/
// 0x10=eventDay ⇒ 原码 (hour, rate, eventDay) 与格式串一一对应；旧写 (eventDay, rate,
// hour) 使日志 hour/event_day 两值互换。改序后 4.4 edi/esi/ebx 分配逐条复现。
#include "TaiwanDBDispatcher/dispatch_2.inc"

// ORIG 0x0815c034：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id from
// event_create_dnf_user where occ_date=CURDATE() and m_id=%s",
// NumberToString(msg->m_id))；exec 失败 throw "select_exec"；
// received = (get_n_rows() != 0)；InterSelectCreateDnfUserInfo::makeRequest(uid,
// msg->m_id, received)；cMyTrace(0x24d)("[Taiwan, Event] select
// event_create_dnf_user. m_id:%s isReceived:%d", str, received)；catch
// logFormat(0x253) 返 0。成功返 1。
// [b28d-lane6] AE（已修）：删 set_query 返回值检查（ORIG 0x0815c034 仅 check exec，多出的 throw 块消除）。
#include "TaiwanDBDispatcher/dispatch_3.inc"

// ORIG 0x0815c25c：GetRepeatEvent(0xa0) 非空且 IsEventing((CUser*)0)==1 才
// 发消息：Acquire(file, 0x261) → CStreamGuard(true) → internal_stream(guard,
// 0xb, a) → GetInBuffer<MSG> null → cMyTrace(0x267)("[Taiwan, Event] msg is
// null")；否则 *buf = b；MsgQueueMgr::put(DB_Q, guard)。
// [b28d-lane6] AE（已修）：去 bool skip 汇聚槽改早期返回 + P221 临时链（复现 ORIG 0x0815c25c 帧 0x30/槽序）。
#include "TaiwanDBDispatcher/makeRequest_2.inc"

// ORIG 0x0815c3b0：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("inSert into
// event_create_dnf_user(occ_date,m_id,server_id,occ_time) VALUES(CURDATE(),%s,
// %d,CURTIME())", NumberToString(msg->m_id), G_CEnvironment()->+0x378
// (m_serverGroup))；exec 失败 throw "insert_exec"；cMyTrace(0x281)
// ("[Taiwan, Event] insert event_create_dnf_user. m_id:%d", msg->m_id)；catch
// logFormat(0x286) 返 0。成功返 1。
// [b28d-lane6] AE（已修）：P221 临时链 + +0x378 视图内联（ORIG 0x0815c3b0 单条 mov 0x378(%eax),%ebx）。
#include "TaiwanDBDispatcher/dispatch_4.inc"

// ORIG 0x0815c586：user->m_field8ec31 非 0 直接返回；Acquire(file, 0x294) →
// CStreamGuard(true) → internal_stream(guard, 0xc, user->GetUID()) →
// GetInBuffer<MSG> null → cMyTrace(0x29a)("[Taiwan, MobileAuth] msg is null")；
// 否则 *buf = user->get_acc_id()；MsgQueueMgr::put(DB_Q, guard)。
#include "TaiwanDBDispatcher/makeRequest_3.inc"

// ORIG 0x0815c6a8：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id from
// mobile_auth_reward_tw where m_id=%s", NumberToString(msg->m_id))；exec 失败
// throw "select_exec"；get_n_rows()==0 → 插行 exec 失败 throw "insert_exec"、
// ok=false、cMyTrace(0x2be) 插行日志；否则 ok=true、cMyTrace(0x2c3) 查行日志；
// InterSelectMobileAuthReward::makeRequest(uid, msg->m_id, ok)；catch
// logFormat(0x2cd) 返 0。成功返 1。
// [b28d-solo-TaiwanDBDispatcher] AE（171/171）：删 mId 具名缓存（P183/P0，ORIG 每处
// 直读 `mov -0x14(%ebp),%eax; mov (%eax),%eax`）后帧 0x60→0x50、bool 槽 -0x19→-0x15、
// trace 临时槽 -0x3c→-0x38 全归零。
#include "TaiwanDBDispatcher/dispatch_5.inc"

// ORIG 0x0815ca94：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id, remain_reward_count,
// unix_timestamp(occ_date) FROM pcroom_daily_reward_tw WHERE m_id=%s",
// NumberToString(msg->m_id), G_CEnvironment()->+0x37c (m_fatigueTime))；exec
// 失败 throw "select_exec"；get_n_rows()==0 → makeRequest(uid, 0, 4) +
// cMyTrace(0x300)；否则 fetch 失败 throw "fetch"；get_int(0,&yesterday) 失败
// throw "get_int 0"；get_byte(1,&flag) 失败 throw "get_int 1"；
// get_int(2,&reward) 失败 throw "get_int 2"；
// CheckDailyScheduleTime(fatigueTime, reward, getCurSec()) 为真 →
// makeRequest(uid, yesterday, 4) + cMyTrace(0x311)；否则 makeRequest(uid,
// yesterday, (char)flag) + cMyTrace(0x316)。catch logFormat(0x31c) 返 0。
// 成功返 1。
// [b28d-solo-TaiwanDBDispatcher] AE（261/261）：三处修形——①补 ORIG 的 dead bool 局部
// （P171/P232：0x0815ca9c `movb $0x1,-0x15(%ebp)` 写后从不读）⇒ 槽基 -0x48→-0x4c 归零；
// ②yesterday/reward/flag 声明移到 GetOutBuffer 之后（P28：ORIG 0x0815cae0 三条清零位置）；
// ③return 1 移出 try 到函数尾（P214②：ORIG 0x0815cef7 `mov $0x1,%ebx` 与 catch 的
// `mov $0x0,%ebx` 在尾共享块汇合）。
#include "TaiwanDBDispatcher/dispatch_6.inc"

// ORIG 0x0815d024：GetOutBuffer<MSG> null → throw "context null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id from
// pcroom_daily_reward_tw where m_id=%s", NumberToString(msg->m_id))；exec 失败
// throw "select_exec"；get_n_rows()==0 → INSERT(values('%s', now(), %d),
// str, (char)msg->m_remain) exec 失败 throw "insert_exec"；否则 UPDATE
// (remain_reward_count=%d where m_id = %s, str, (char)m_remain) exec 失败
// throw "update_exec"。catch logFormat(0x35d) 返 0。成功返 1。
// [b28d-lane6] AE（已修·真 bug）：UPDATE 语句实参序 (str,remain)→(remain,str) 修正（ORIG 0x0815d0f2 压栈序实证）+ 删多余 set_query 检查 + remain 内联。
#include "TaiwanDBDispatcher/dispatch_7.inc"

// ORIG 0x0815d3ee：GetOutBuffer<MSG> null → throw "context null"；GetDBHandle
// (9,0) null → throw "handle null"；time+localtime_r 拷贝 tm，tm_hour 覆为
// m_fatigueTime、tm_min/tm_sec 清零，mktime；原始 tm_hour < fatigueTime 时减
// 86400；set_query("seLect m_id from event_1306_account_reward_2nd where m_id=%s
// and occ_date > FROM_UNIXTIME(%d)", str, now) exec 失败 throw "select_exec"；
// get_n_rows()==0 → INSERT(m_id,charac_no,occ_date) exec 失败 throw
// "insert_exec"；Acquire(file, 0x3a5) → CStreamGuard(true) →
// internal_stream(guard, 0x10, uid) → GetInBuffer<MSG> null → throw "stream"；
// 拷贝 m_characNo/m_accId 后 MsgQueueMgr::put(idx 1, guard)。catch
// logFormat(0x3b4) 返 0。成功返 1。
// [b21] 修正三真 bug：①set_query 返回不检查（ORIG 仅 check exec，删 if(!..)throw 两支）；
//   ②MsgQueueMgr::put 旧用 DB_Q(=2)，ORIG 传 $0x1（同族 Taiwan dispatch 均 idx 1，改 DISPATCH_Q）；
//   ③m_accId/m_characNo 拷贝序反（ORIG 先 0x4 后 0x0）。疲劳比较改读 tm2.tm_hour 内存对
//   齐 `cmp/jge`；Acquire 内联进 CStreamGuard ctor（P134）消中间槽；nRows 具名局部。
// [b28c-L20] AE — 消上列两处残余（原「封 N82」判词作废）：
//   ①m_characNo 装载：写 (unsigned int)sig->m_characNo 的异型化实参使 4.4 多
//   `mov %eax,%ebx` 中转；保持字段 int 同型直传即复现 ORIG `mov 0x4(%eax),%ebx`（P211 反向）；
//   ②返回路径：ORIG 成功值 `mov $1,%ebx` 落在函数尾共享块（0x0815d797，guard 析构之后、
//   与 catch 的 mov $0,%ebx 汇合）⇒ return 1 必须写在 try 之外。
//   0x0815d3ee orig=231 our=231 gap=0 ndiff=0。
#include "TaiwanDBDispatcher/dispatch_8.inc"

// ORIG 0x0815d7a8：GetOutBuffer<Taiwan::SigLoadHeroMissionData> null → throw
// "context_null"；GetDBHandle(9,0) null → throw "handle_null"；set_query
// ("seLect mission from event_hero_mission_data where server_group=%d and
// charac_no=%u", get_server_group(), sig->m_field0) exec 失败 throw
// "exec_select"；SigLoadHeroMissionData data; data.init(); data.m_field0 =
// sig->m_field0；get_n_rows()==0 → INSERT(values(%d,%u,'',now())) exec 失败
// throw "exec_insert"、memset(data+4,0,0x100)；否则 fetch 失败 throw "fetch"、
// get_binary(0, data+4, 0x100) 失败 throw "get_binary"；Acquire(file, 0x3ea)
// → CStreamGuard(true) → internal_stream(guard, 0x12, uid) →
// GetInBuffer<SigLoadHeroMissionData> null → throw "stream_alloc"；
// memcpy(out, &data, 0x104)；MsgQueueMgr::put(DB_Q, guard)。catch
// logFormat(0x3f5) 返 0。成功返 1。
// [b28d-solo-TaiwanDBDispatcher] AE（231/231）：两处修形——①SigLoadHeroMissionData
// 字段 public 化 + 改直接成员访问（§8.3；ORIG 0x0815d7a8 的 `mov -0x18(%ebp),%eax;
// mov (%eax),%eax; mov %eax,-0x124(%ebp)` 是 data.m_field0=sig->m_field0 的 disp 直存，
// 旧 cast 视图形恒 lea+LHS-first；同一改动连带 memset/get_binary 实参序归零）；
// ②`return put(DISPATCH_Q, guard)`（ORIG 0x0815daf3 `call put; mov %eax,%ebx` 尾
// epilogue 返回 ebx；旧写 put();return 1 在 put 失败时恒返 1 = 真语义 bug）。
#include "TaiwanDBDispatcher/dispatch_9.inc"

// ORIG 0x0815db98：GetOutBuffer<Taiwan::SigSaveHeroMissionData> null → throw
// "context_null"；GetDBHandle(9,0) null → throw "handle_null"；
// blob_to_str(0, sig+4, 0x100)；set_query("upDate event_hero_mission_data set
// mission='%s' where server_group=%d and charac_no=%u", blob,
// get_server_group(), sig->m_field0) exec 失败 throw "exec_update"。catch
// logFormat(0x413) 返 0。成功返 1。
// [b28d-lane6] AE（已修）：三实参内联（blob/serverGroup/characNo 不设具名槽）后
// 帧 0x30 与 ORIG 0x0815db98 全等（104=104）。
#include "TaiwanDBDispatcher/dispatch_10.inc"
// ---- OnlyOrigBatch10：Taiwan DB 域 dispatch 实现 ----

// ORIG 0x0815b010 T LoadGeolocation::dispatch（报告 LoadGeolocation/dispatch.md）：
// GetDBHandle(1,0) null→throw "handle null"；get_server_group → set_query
// geo_allow_country → exec 失败 throw "exec"；逐行 get_str(0,buf,0x10) 失败 throw
// "get_str" → SigLoadGeolocation::set；count>0x2b 时 sendCountryData+init；末尾
// sendCountryData 一次。catch(const char*) logFormat 0x13c 返 0，成功返 1。
#include "TaiwanDBDispatcher/dispatch_11.inc"

// ORIG 0x0815b266 T LoadGeolocation::sendCountryData（报告 sendCountryData.md）：
// Acquire(...,0x144) → internal_stream(guard, 4, -1) → GetInBuffer<SigLoadGeolocation>
// null→throw "stream" → setData → put(Q1)。
#include "TaiwanDBDispatcher/sendCountryData.inc"

// ORIG 0x0815a8f2 T LoadServiceRestrictPolicy::dispatch（报告 dispatch.md）：
// GetDBHandle(1,0) null→throw "handle null"；set_query 失败 throw "set_query"；
// exec 失败 throw "exec"；逐行 get_int(++idx,&v)/get_char(&c) 失败 throw
// "category"/"restrict"/"values"；sig.restrictValue==0 或 ==v1 时 sig.set(v2,v1,c)，
// 否则先 sendRestrictData+init 再 set；末尾 sendRestrictData。catch 0xd2 返 0。
// [b28d-solo-TaiwanDBDispatcher] DIFF·N 封顶（201/201，仅 3 处实参求值序）：①真 bug
// 修正 set(v2,v1,c)→set(v1,v2,c)（ORIG 0x0815a8f2 两处压栈序 0x4=v1(-0x1c)/0x8=v2
// (-0x20)/0xc=c 实证；旧写使 m_restrictValue 与 m_extra 索引互换）；②TaiwanQueries.h
// SigLoadRestrictPolicy 加 pack(1)（P250：ORIG 槽基 -0x1221 非 4 对齐）⇒ 槽基/块序归零。
// 残余：三处 get_int/get_char 实参求值序——ORIG 左→右（idx 先入 %eax、&v 后入 %edx），
// 4.4 恒右→左（先 lea &v）；实测语句表达式/逗号/(int)/(unsigned)/引用 cast/内联 ++idx/
// idx+=1 七形均不可翻（(short)idx 可对齐求值序但多 3 条 cwtl 且引入截断，弃用）；
// 实参值/压栈序/调用目标全同 ⇒ P75 跨版本实参装载时刻，语义等价封 N。
#include "TaiwanDBDispatcher/dispatch_12.inc"

// ORIG 0x0815ac40 T LoadServiceRestrictPolicy::sendRestrictData（报告
// sendRestrictData.md）：Acquire(...,0xd9) → internal_stream(guard, 1, -1) →
// GetInBuffer<SigLoadRestrictPolicy> null→throw "stream" → set → put(Q1)。
#include "TaiwanDBDispatcher/sendRestrictData.inc"

// ORIG 0x0815ad3c T SaveServiceRestrictPolicy::dispatch（报告 dispatch.md）：
// GetOutBuffer<SigLoadRestrictPolicy> null→throw "context null"；GetDBHandle(1,0)
// null→throw "handle null"；两循环（i=1..sig->m_count）：dnf_restrict_state
// values(%d,%d,%d,'%d')= (sg, restrictValue, i, m_extra[i])；dnf_restrict_info
// values(%d,%d,'%s')= (sg, restrictValue, i, &m_data[i*0x2d]+8)；set_query 失败
// throw "set_query" / exec 失败 throw "insert_exec"。catch 0x10e 返 0。
#include "TaiwanDBDispatcher/dispatch_13.inc"

// ORIG 0x0815b76a T ResetStayTimeEvent::dispatch（报告 dispatch.md）：
// GetDBHandle(9,0) null→throw "handle null"；set_query("upDate
// event_stay_time_charac set reward_flag = 0")（不检查）→ exec 失败 throw "exec"。
// catch 0x1ac 返 0，成功返 1。
#include "TaiwanDBDispatcher/dispatch_14.inc"
