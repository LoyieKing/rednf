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

// ---- DB* 子类最小实现（仅基类 ctor + vptr；业务方法不在本批）----

LoadServiceRestrictPolicy::LoadServiceRestrictPolicy() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)1)  // ORIG @0x0815e001 movl $0x1
{
}
LoadServiceRestrictPolicy::~LoadServiceRestrictPolicy()
{
}

SaveServiceRestrictPolicy::SaveServiceRestrictPolicy() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)2)  // ORIG @0x0815e073 movl $0x2
{
}
SaveServiceRestrictPolicy::~SaveServiceRestrictPolicy()
{
}

LoadGeolocation::LoadGeolocation() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)4)  // ORIG @0x0815e0e5 movl $0x4
{
}
LoadGeolocation::~LoadGeolocation()
{
}

CheckStayTimeEvent::CheckStayTimeEvent() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)5)  // ORIG @0x0815e157 movl $0x5
{
}
CheckStayTimeEvent::~CheckStayTimeEvent()
{
}

ResetStayTimeEvent::ResetStayTimeEvent() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)7)  // ORIG @0x0815e1c9 movl $0x7
{
}
ResetStayTimeEvent::~ResetStayTimeEvent()
{
}

DBUpdateExitTime::DBUpdateExitTime() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)8)  // ORIG code 0x8
{
}
DBUpdateExitTime::~DBUpdateExitTime()
{
}


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
bool DBUpdateExitTime::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigUpdateExitTime* sig =
            stream->GetOutBuffer<Taiwan::SigUpdateExitTime>();
        if (sig == 0)
            throw "handle null";
        if (GlobalData::s_psimpleSSO->updateSSOExpireTime(
                reinterpret_cast<DBUpdateExitTimeView*>(sig)->m_timeA,
                reinterpret_cast<DBUpdateExitTimeView*>(sig)->m_timeB) != 1)
            throw "exec";
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x1d4,
            "[Taiwan, Auth] update member_login last_play_time. (error:%s)", e);
        return 0;
    }
}

DBSelectCreateDnfEventInfo::DBSelectCreateDnfEventInfo() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)9)  // ORIG @0x0815e2ad movl $0x9
{
}
DBSelectCreateDnfEventInfo::~DBSelectCreateDnfEventInfo()
{
}

DBSelectCreateDnfUserInfo::DBSelectCreateDnfUserInfo() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0xa)  // ORIG @0x0815e31f movl $0xa
{
}
DBSelectCreateDnfUserInfo::~DBSelectCreateDnfUserInfo()
{
}

DBInsertCreateDnfUserInfo::DBInsertCreateDnfUserInfo() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0xb)  // ORIG @0x0815e391 movl $0xb
{
}
DBInsertCreateDnfUserInfo::~DBInsertCreateDnfUserInfo()
{
}

DBSelectMobileAuthRewardTw::DBSelectMobileAuthRewardTw() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0xc)  // ORIG @0x0815e403 movl $0xc
{
}
DBSelectMobileAuthRewardTw::~DBSelectMobileAuthRewardTw()
{
}

DBSelectPcroomDailyReward::DBSelectPcroomDailyReward() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0xd)  // ORIG @0x0815e475 movl $0xd
{
}
DBSelectPcroomDailyReward::~DBSelectPcroomDailyReward()
{
}

DBUpdatePcroomDailyReward::DBUpdatePcroomDailyReward() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0xe)  // ORIG @0x0815e4e7 movl $0xe
{
}
DBUpdatePcroomDailyReward::~DBUpdatePcroomDailyReward()
{
}

// ORIG T 0x0815cf06（报告 DBUpdatePcroomDailyReward/makeRequest.md）：PC 房日奖励
// 落库请求：internal_stream(0xe, uid) → MSG{m_id=acc_id, m_remain=user+0x8ec32}。
// [b22判词·N82槽号] 75/75 助记符序列逐条一致，残余仅局部栈槽编号差（guard
// -0x24/-0x14、cMyTrace -0x1c/-0x24）：ORIG 声明序与 gcc4.4.7 现序的帧槽分配差，
// 不可由源码复现；relink 后 call 占位消除即为 NEAR，语义等价。
void DBUpdatePcroomDailyReward::makeRequest(CUser* user)
{
    // P0：Acquire 返回值右值内联进 ctor（ORIG %eax 直通，无 stream 栈槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x325), true);
    Taiwan::internal_stream(guard, TaiwanInternalPack::T(0xe), user->GetUID());
    MSG* msg = guard->GetInBuffer<MSG>();   // ORIG via CStreamGuard::operator->
    if (msg == 0)
    {
        cMyTrace trace(
            "static void DBUpdatePcroomDailyReward::makeRequest(CUser*)",
            0x32b, 0);
        trace("[Taiwan, Pcroom] msg is null");
    }
    else
    {
        msg->m_id = user->get_acc_id();
        msg->m_remain = user->m_field8ec32;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

DBInsertGiveBoxEventReward::DBInsertGiveBoxEventReward() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0x10)  // ORIG @0x0815e559 movl $0x10
{
}
DBInsertGiveBoxEventReward::~DBInsertGiveBoxEventReward()
{
}

DBLoadHeroMissionEventData::DBLoadHeroMissionEventData() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0x12)  // ORIG @0x0815e5cb movl $0x12
{
}
DBLoadHeroMissionEventData::~DBLoadHeroMissionEventData()
{
}

DBSaveHeroMissionEventData::DBSaveHeroMissionEventData() throw()
    : TaiwanInterfaceDBDispatcher((TaiwanInternalPack::T)0x11)  // ORIG @0x0815e63d movl $0x11
{
}
DBSaveHeroMissionEventData::~DBSaveHeroMissionEventData()
{
}

// ---- TaiwanInterfaceDBDispatcher 基类（ORIG W：C1 0x0815df66 / D1 0x0815df7e / D0 0x0815dfa4 / getCode 0x0815dfc2）----

TaiwanInterfaceDBDispatcher::TaiwanInterfaceDBDispatcher(TaiwanInternalPack::T code) throw()
    : m_code(code)
{
}
TaiwanInterfaceDBDispatcher::~TaiwanInterfaceDBDispatcher()
{
}
TaiwanInternalPack::T TaiwanInterfaceDBDispatcher::getCode() const
{
    return m_code;
}

// ---- TaiwanDBDispatcher ----

TaiwanDBDispatcher::TaiwanDBDispatcher()
{
    m_dispatchers.clear();   // ORIG @0x0815a134：map::map + map::clear
}

TaiwanDBDispatcher::~TaiwanDBDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), FnDelete());  // ORIG @0x0815a176
}

void TaiwanDBDispatcher::FnDelete::operator()(
    std::pair<const TaiwanInternalPack::T, TaiwanInterfaceDBDispatcher*>& item)
{
    delete item.second;   // ORIG @0x0815dfce：test+je + vtable D0 调用
}

TaiwanInterfaceDBDispatcher* TaiwanDBDispatcher::getDispatcher(TaiwanInternalPack::T code)
{
    std::map<TaiwanInternalPack::T, TaiwanInterfaceDBDispatcher*>::iterator it =
        m_dispatchers.find(code);
    if (it != m_dispatchers.end())
    {
        return it->second;
    }
    return 0;
}

bool TaiwanDBDispatcher::addDispatcher(TaiwanInternalPack::T code,
                                       TaiwanInterfaceDBDispatcher* disp)
{
    if (getDispatcher(code) != 0)
    {
        return false;
    }
    m_dispatchers[code] = disp;
    return true;
}

void TaiwanDBDispatcher::delDispatcher(TaiwanInternalPack::T code)
{
    if (getDispatcher(code) != 0)
    {
        m_dispatchers.erase(m_dispatchers.find(code));
    }
}

bool TaiwanDBDispatcher::init()
{
    TaiwanInterfaceDBDispatcher* p = 0;
    p = new LoadServiceRestrictPolicy();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new SaveServiceRestrictPolicy();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new LoadGeolocation();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new CheckStayTimeEvent();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new ResetStayTimeEvent();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBUpdateExitTime();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectCreateDnfEventInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectCreateDnfUserInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBInsertCreateDnfUserInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectMobileAuthRewardTw();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSelectPcroomDailyReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBUpdatePcroomDailyReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBInsertGiveBoxEventReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBLoadHeroMissionEventData();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    p = new DBSaveHeroMissionEventData();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }
    return true;
}

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
bool CheckStayTimeEvent::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        // ORIG 0x0815b362：GetOutBuffer<Taiwan::SigStayTimeEvent>（MoreTypes.h 真类型；
        // 禁本地同构视图——实例化类型不符产生 EXTRA 调用，P167 判据）。
        Taiwan::SigStayTimeEvent* sig =
            stream->GetOutBuffer<Taiwan::SigStayTimeEvent>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x0815b424：set_query 返回值不检查（失败留给 exec 的 throw）
        db->set_query(
            "seLect reward_flag from event_stay_time_charac where m_id=%s",
            NumberToString(sig->m_accId, 0));
        if (!db->exec(true))                 // ORIG：exec(true) 失败 throw
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            // ORIG 0x0815b4ac：insert set_query 不检查，exec(true) 失败 throw
            db->set_query(
                "inSert into event_stay_time_charac values('%s', 1, 1, now())",
                NumberToString(sig->m_accId, 0));
            if (!db->exec(true))
                throw "insert_exec";
            sendStayTimeData(uid);
        }
        else
        {
            if (!db->fetch())
                throw "fetch";
            int flag = 0;
            if (!db->get_int(0, flag))
                throw "get_int";
            // ORIG 0x0815b5a1：`test; jne 0x2ce`（flag!=0 直跳 return 块），
            // Ghidra `if (flag==0) { update } return 1`。原 `if(flag!=0)return 1`
            // 被 4.4 反成 je+mov$1（+4 条）。改 flag==0 正入 update、return 1
            // 统一走末尾（P59）。
            if (flag == 0)
            {
                // ORIG：update set_query 不检查，exec(true) 失败 throw
                db->set_query(
                    "upDate event_stay_time_charac set reward_flag=1, reward_count=reward_count+1, mod_date=now() where m_id = %s",
                    NumberToString(sig->m_accId, 0));
                if (!db->exec(true))
                    throw "update_exec";
                sendStayTimeData(uid);
            }
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x18a,
            "[Taiwan, Event] select event_stay_time_charac. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815b6ac：Acquire("localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x192)
// → CStreamGuard(true) → internal_stream(guard, 0x5, uid) →
// GetInBuffer<Taiwan::SigStayTimeEvent> 非空时 MsgQueueMgr::put(DB_Q, guard)。
void CheckStayTimeEvent::sendStayTimeData(int uid)
{
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x192), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x5, uid);
    Taiwan::SigStayTimeEvent* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigStayTimeEvent>();
    if (sig != 0)
    {
        // [b21] ORIG put @0x815b735 movl $0x1=DISPATCH_Q（QueueSweep21 回扫实证）；
        // 非 DB_Q(2)——同族 DB 派发器归队队列 idx 1。
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}

// ORIG 0x0815b89a（static）：Acquire(file, 0x1b9) → CStreamGuard(true) →
// internal_stream(guard, 0x8, a) → GetInBuffer<Taiwan::SigUpdateExitTime> 非空
// 时 init()、set(a, c)（第三参 b 未用）、MsgQueueMgr::put(DB_Q, guard)。
void DBUpdateExitTime::makeRequest(unsigned int a, int b, unsigned int c)
{
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x1b9), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x8, a);
    Taiwan::SigUpdateExitTime* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigUpdateExitTime>();
    if (sig != 0)
    {
        sig->init();
        sig->set(a, c);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG 0x0815bb8c：GetDBHandle(9,0) null → throw "handle null"；
// set_query("seLect rate, HOUR(CURTIME()), event_day FROM event_create_dnf_info
// WHERE occ_time=DATE_ADD(CURDATE(), INTERVAL HOUR(CURTIME()) HOUR)")；exec 失败
// throw "select_exec"；get_n_rows()==0 throw "get_n_rows=0"；fetch 失败 throw
// "fetch"；get_int(0/1/2) 失败 throw "get_int N"；
// InterSelectCreateDnfEventInfo::makeRequest(rate,hour,eventDay)；
// cMyTrace(0x20d)("[Taiwan, Event] select event_create_dnf_info. hour:%d rate:%d
// event_day:%d", c, a, b)；catch：getCurSec→localtime_r→makeRequest(-1,
// tm_hour, 0)，logFormat(0x219) 返 0。成功返 1。
bool DBSelectCreateDnfEventInfo::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        int rate = 0;
        int hour = 0;
        int eventDay = 0;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG：set_query 返回值不检查（仅 exec 检查）
        db->set_query(
            "seLect rate, HOUR(CURTIME()), event_day FROM event_create_dnf_info WHERE occ_time=DATE_ADD(CURDATE(), INTERVAL HOUR(CURTIME()) HOUR)");
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
            throw "get_n_rows=0";
        if (!db->fetch())
            throw "fetch";
        if (!db->get_int(0, rate))
            throw "get_int 0";
        if (!db->get_int(1, hour))
            throw "get_int 1";
        if (!db->get_int(2, eventDay))
            throw "get_int 2";
        InterSelectCreateDnfEventInfo::makeRequest(rate, hour, eventDay);
        cMyTrace t(__PRETTY_FUNCTION__, 0x20d, 0);
        t("[Taiwan, Event] select event_create_dnf_info. hour:%d rate:%d event_day:%d",
          eventDay, rate, hour);
        return 1;
    }
    catch (const char* e)
    {
        int sec = GlobalData::s_systemTime_.getCurSec();
        struct tm tm;
        localtime_r((time_t*)&sec, &tm);
        InterSelectCreateDnfEventInfo::makeRequest(-1, tm.tm_hour, 0);
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x219,
            "[Taiwan, Event] select event_create_dnf_event_info. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815c034：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id from
// event_create_dnf_user where occ_date=CURDATE() and m_id=%s",
// NumberToString(msg->m_id))；exec 失败 throw "select_exec"；
// received = (get_n_rows() != 0)；InterSelectCreateDnfUserInfo::makeRequest(uid,
// msg->m_id, received)；cMyTrace(0x24d)("[Taiwan, Event] select
// event_create_dnf_user. m_id:%s isReceived:%d", str, received)；catch
// logFormat(0x253) 返 0。成功返 1。
bool DBSelectCreateDnfUserInfo::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBSelectCreateDnfUserInfo::MSG* sig = stream->GetOutBuffer<DBSelectCreateDnfUserInfo::MSG>();
        if (sig == 0)
            throw "msg is null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                "seLect m_id from event_create_dnf_user where occ_date=CURDATE() and m_id=%s",
                NumberToString(sig->m_id, 0)))
            throw "select_exec";
        if (!db->exec(1))
            throw "select_exec";
        bool received = false;
        if (db->get_n_rows() != 0)
            received = true;
        InterSelectCreateDnfUserInfo::makeRequest(uid, sig->m_id, received);
        cMyTrace t(__PRETTY_FUNCTION__, 0x24d, 0);
        t("[Taiwan, Event] select event_create_dnf_user. m_id:%s isReceived:%d",
          NumberToString(sig->m_id, 0), received ? 1 : 0);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x253,
            "[Taiwan, Event] select event_create_dnf_user. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815c25c：GetRepeatEvent(0xa0) 非空且 IsEventing((CUser*)0)==1 才
// 发消息：Acquire(file, 0x261) → CStreamGuard(true) → internal_stream(guard,
// 0xb, a) → GetInBuffer<MSG> null → cMyTrace(0x267)("[Taiwan, Event] msg is
// null")；否则 *buf = b；MsgQueueMgr::put(DB_Q, guard)。
void DBInsertCreateDnfUserInfo::makeRequest(int a, unsigned int b)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    bool skip = true;
    if (ev != 0)
    {
        if (ev->IsEventing((CUser*)0))
            skip = false;
    }
    if (!skip)
    {
        // P89：单一视图表达式（合并 stream 局部）
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x261), true);
        Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0xb, a);
        DBInsertCreateDnfUserInfo::MSG* sig =
            guard.operator->()->GetInBuffer<DBInsertCreateDnfUserInfo::MSG>();
        if (sig == 0)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x267, 0);
            t("[Taiwan, Event] msg is null");
        }
        else
        {
            *reinterpret_cast<unsigned int*>(sig) = b;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
    }
}

// ORIG 0x0815c3b0：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("inSert into
// event_create_dnf_user(occ_date,m_id,server_id,occ_time) VALUES(CURDATE(),%s,
// %d,CURTIME())", NumberToString(msg->m_id), G_CEnvironment()->+0x378
// (m_serverGroup))；exec 失败 throw "insert_exec"；cMyTrace(0x281)
// ("[Taiwan, Event] insert event_create_dnf_user. m_id:%d", msg->m_id)；catch
// logFormat(0x286) 返 0。成功返 1。
bool DBInsertCreateDnfUserInfo::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBInsertCreateDnfUserInfo::MSG* sig = stream->GetOutBuffer<DBInsertCreateDnfUserInfo::MSG>();
        if (sig == 0)
            throw "msg is null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x0815c480: set_query("inSert into event_create_dnf_user(occ_date,
        //   m_id,server_id,occ_time) VALUES(CURDATE(),%s,%d,CURTIME())",
        //   NumberToString(sig->m_id,0), G_CEnvironment()->+0x378)
        const int serverGroup = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
        db->set_query(
            "inSert into event_create_dnf_user(occ_date,m_id,server_id,occ_time) "
            "VALUES(CURDATE(),%s,%d,CURTIME())",
            NumberToString(*reinterpret_cast<unsigned int*>(sig), 0),
            serverGroup);
        if (!db->exec(1))
            throw "insert_exec";
        cMyTrace t(__PRETTY_FUNCTION__, 0x281, 0);
        t("[Taiwan, Event] insert event_create_dnf_user. m_id:%d",
          *reinterpret_cast<unsigned int*>(sig));
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x286,
            "[Taiwan, Event] insert event_create_dnf_user. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815c586：user->m_field8ec31 非 0 直接返回；Acquire(file, 0x294) →
// CStreamGuard(true) → internal_stream(guard, 0xc, user->GetUID()) →
// GetInBuffer<MSG> null → cMyTrace(0x29a)("[Taiwan, MobileAuth] msg is null")；
// 否则 *buf = user->get_acc_id()；MsgQueueMgr::put(DB_Q, guard)。
void DBSelectMobileAuthRewardTw::makeRequest(CUser* user)
{
    if (user->m_field8ec31 != 0)
        return;
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x294), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0xc, user->GetUID());
    DBSelectMobileAuthRewardTw::MSG* sig =
        guard.operator->()->GetInBuffer<DBSelectMobileAuthRewardTw::MSG>();
    if (sig == 0)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x29a, 0);
        t("[Taiwan, MobileAuth] msg is null");
    }
    else
    {
        *reinterpret_cast<unsigned int*>(sig) = user->get_acc_id();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG 0x0815c6a8：GetOutBuffer<MSG> null → throw "msg is null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id from
// mobile_auth_reward_tw where m_id=%s", NumberToString(msg->m_id))；exec 失败
// throw "select_exec"；get_n_rows()==0 → 插行 exec 失败 throw "insert_exec"、
// ok=false、cMyTrace(0x2be) 插行日志；否则 ok=true、cMyTrace(0x2c3) 查行日志；
// InterSelectMobileAuthReward::makeRequest(uid, msg->m_id, ok)；catch
// logFormat(0x2cd) 返 0。成功返 1。
bool DBSelectMobileAuthRewardTw::dispatch(int code, int uid, Stream* stream)
{
    // [b21 N] 语义闭合：msg-null/handle-null throw、set_query(不查返)+exec→select_exec、
    // get_n_rows()==0 → INSERT+ok=false+trace(0x2be)、else ok=true+trace(0x2c3)、
    // makeRequest(uid, m_id, ok)、return 1、catch logFormat(0x2cd) 返 0——逐条对齐 ORIG。
    //   注意 set_query 两处均为裸调用不查返（ORIG 仅 check exec，同族 GiveBox 已修）。
    //   残余 N82/N86：ORIG 逐用 `*(uint*)sig` 直读（无 mId 命名缓存），4.4 存 mId 局部；
    //   throw 字符串/typeinfo 地址 .o 重定位产物。无子母，封 N。
    try
    {
        bool ok = true;
        DBSelectMobileAuthRewardTw::MSG* sig = stream->GetOutBuffer<DBSelectMobileAuthRewardTw::MSG>();
        if (sig == 0)
            throw "msg is null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        unsigned int mId = *reinterpret_cast<unsigned int*>(sig);
        // ORIG：set_query 返回值不检查（仅 exec 检查）
        db->set_query("seLect m_id from mobile_auth_reward_tw where m_id=%s",
                      NumberToString(mId, 0));
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into mobile_auth_reward_tw(m_id,occ_date) VALUES(%s,now())",
                NumberToString(mId, 0));
            if (!db->exec(1))
                throw "insert_exec";
            ok = false;
            cMyTrace t(__PRETTY_FUNCTION__, 0x2be, 0);
            t("[Taiwan, MobileAuth] insert mobile_auth_reward_tw. m_id:%s",
              NumberToString(mId, 0));
        }
        else
        {
            ok = true;
            cMyTrace t(__PRETTY_FUNCTION__, 0x2c3, 0);
            t("[Taiwan, MobileAuth] select mobile_auth_reward_tw. m_id:%s",
              NumberToString(mId, 0));
        }
        InterSelectMobileAuthReward::makeRequest(uid, mId, ok);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x2cd,
            "[Taiwan, MobileAuth] mobile_auth_reward_tw. (error:%s)", e);
        return 0;
    }
}

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
bool DBSelectPcroomDailyReward::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        unsigned int yesterday = 0;
        unsigned int reward = 0;
        char flag = 0;
        DBSelectPcroomDailyReward::MSG* sig = stream->GetOutBuffer<DBSelectPcroomDailyReward::MSG>();
        if (sig == 0)
            throw "msg is null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x0815cb49：G_CEnvironment()+0x37c 供 set_query（不检查返回值）；
        // 0x0815cd86 再调一次供 CheckDailyScheduleTime（两次独立 G_CEnvironment）。
        db->set_query(
            "seLect m_id, remain_reward_count, unix_timestamp(occ_date) FROM pcroom_daily_reward_tw WHERE m_id=%s",
            NumberToString(*reinterpret_cast<unsigned int*>(sig), 0),
            G_CEnvironment()->m_serverEnvir.m_fatigueTime);
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            InterSelectPcroomDailyReward::makeRequest(uid, yesterday, 4);
            cMyTrace t(__PRETTY_FUNCTION__, 0x300, 0);
            t("[Taiwan, Pcroom] pcroom_daily_reward_tw get_n_row==0 m_id:%s",
              NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
        }
        else
        {
            if (!db->fetch())
                throw "fetch";
            if (!db->get_int(0, yesterday))
                throw "get_int 0";
            if (!db->get_byte(1, flag))
                throw "get_int 1";
            if (!db->get_int(2, reward))
                throw "get_int 2";
            if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                       reward,
                                       GlobalData::s_systemTime_.getCurSec()))
            {
                InterSelectPcroomDailyReward::makeRequest(uid, yesterday, 4);
                cMyTrace t(__PRETTY_FUNCTION__, 0x311, 0);
                t("[Taiwan, Pcroom] pcroom_daily_reward_tw yesterday m_id:%s",
                  NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
            }
            else
            {
                InterSelectPcroomDailyReward::makeRequest(uid, yesterday,
                                                          (char)flag);
                cMyTrace t(__PRETTY_FUNCTION__, 0x316, 0);
                t("[Taiwan, Pcroom] pcroom_daily_reward_tw today m_id:%s, remain_reward_count:%d",
                  NumberToString(*reinterpret_cast<unsigned int*>(sig), 0),
                  (char)flag);
            }
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x31c,
            "[Taiwan, Pcroom] mobile_auth_reward_tw. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815d024：GetOutBuffer<MSG> null → throw "context null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id from
// pcroom_daily_reward_tw where m_id=%s", NumberToString(msg->m_id))；exec 失败
// throw "select_exec"；get_n_rows()==0 → INSERT(values('%s', now(), %d),
// str, (char)msg->m_remain) exec 失败 throw "insert_exec"；否则 UPDATE
// (remain_reward_count=%d where m_id = %s, str, (char)m_remain) exec 失败
// throw "update_exec"。catch logFormat(0x35d) 返 0。成功返 1。
bool DBUpdatePcroomDailyReward::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBUpdatePcroomDailyReward::MSG* sig = stream->GetOutBuffer<DBUpdatePcroomDailyReward::MSG>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        char remain = sig->m_remain;
        db->set_query("seLect m_id from pcroom_daily_reward_tw where m_id=%s",
                      NumberToString(sig->m_id, 0));
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into pcroom_daily_reward_tw(m_id, occ_date, remain_reward_count) values('%s', now(), %d)",
                NumberToString(sig->m_id, 0), (int)remain);
            if (!db->exec(1))
                throw "insert_exec";
        }
        else
        {
            db->set_query(
                "upDate pcroom_daily_reward_tw set occ_date=now(), remain_reward_count=%d where m_id = %s",
                NumberToString(sig->m_id, 0), (int)remain);
            if (!db->exec(1))
                throw "update_exec";
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x35d,
            "[Taiwan, Pcroom] pcroom_daily_reward_tw. (error:%s)", e);
        return 0;
    }
}

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
//   残余 N82：INSERT 的 m_characNo 装载 ORIG 直 `mov 0x4(%eax),%ebx`，4.4 多 `mov %eax,%ebx`
//   中转；返回路径 %ebx 合并 epilogue 与 ORIG 各 return 点直发略有出入。语义逐一相等，封 N。
bool DBInsertGiveBoxEventReward::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBInsertGiveBoxEventReward::MSG* sig = stream->GetOutBuffer<DBInsertGiveBoxEventReward::MSG>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        time_t t = time(0);
        struct tm tm;
        localtime_r(&t, &tm);
        struct tm tm2 = tm;
        tm2.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        tm2.tm_min = 0;
        tm2.tm_sec = 0;
        time_t now = mktime(&tm2);
        if (tm.tm_hour < tm2.tm_hour)
            now -= 0x15180;
        db->set_query(
                "seLect m_id from event_1306_account_reward_2nd where m_id=%s and occ_date > FROM_UNIXTIME(%d)",
                NumberToString(sig->m_accId, 0), (int)now);
        if (!db->exec(1))
            throw "select_exec";
        int nRows = db->get_n_rows();
        if (nRows == 0)
        {
            db->set_query(
                    "inSert into event_1306_account_reward_2nd (m_id , charac_no , occ_date ) values (%s , %u , now())",
                    NumberToString(sig->m_accId, 0), (unsigned int)sig->m_characNo);
            if (!db->exec(1))
                throw "insert_exec";
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire(
                    "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x3a5),
                true);
            Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x10, uid);
            DBInsertGiveBoxEventReward::MSG* outSig =
                guard.operator->()->GetInBuffer<DBInsertGiveBoxEventReward::MSG>();
            if (outSig == 0)
                throw "stream";
            outSig->m_characNo = sig->m_characNo;
            outSig->m_accId = sig->m_accId;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x3b4,
            "[Taiwan, GiveMeBox] GIVE_ME_BOX_EVENT. (error:%s)", e);
        return 0;
    }
}

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
bool DBLoadHeroMissionEventData::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigLoadHeroMissionData* sig =
            stream->GetOutBuffer<Taiwan::SigLoadHeroMissionData>();
        if (sig == 0)
            throw "context_null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        unsigned int characNo = reinterpret_cast<SigHeroMissionDataView*>(sig)->m_field0;
        // ORIG 0x0815d7a8：G_CEnvironment()+get_server_group() 供 set_query
        // （返回值不检查）；0x0815d82c 插行时再调一次（两次独立调用，P167 判据）。
        // 仅 exec(1) 失败才 throw "exec_select"。
        db->set_query(
            "seLect mission from event_hero_mission_data where server_group=%d and charac_no=%u",
            G_CEnvironment()->get_server_group(), characNo);
        if (!db->exec(1))
            throw "exec_select";
        Taiwan::SigLoadHeroMissionData data;
        data.init();
        reinterpret_cast<SigHeroMissionDataView*>(&data)->m_field0 = characNo;
        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into event_hero_mission_data values(%d,%u,'',now())",
                G_CEnvironment()->get_server_group(), characNo);
            if (!db->exec(1))
                throw "exec_insert";
            memset(reinterpret_cast<SigHeroMissionDataView*>(&data)->m_data, 0, 0x100);
        }
        else
        {
            if (!db->fetch())
                throw "fetch";
            if (!db->get_binary(0, reinterpret_cast<SigHeroMissionDataView*>(&data)->m_data, 0x100))
                throw "get_binary";
        }
        // [b21] ORIG put @0x815daf3 movl $0x1=DISPATCH_Q（QueueSweep21 回扫实证）；
        // 非 DB_Q(2)。Acquire 内联进 ctor（P134）消中间槽。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire(
                "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x3ea),
            true);
        Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x12, uid);
        Taiwan::SigLoadHeroMissionData* outSig =
            guard.operator->()->GetInBuffer<Taiwan::SigLoadHeroMissionData>();
        if (outSig == 0)
            throw "stream_alloc";
        memcpy(outSig, &data, 0x104);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x3f5,
            "[HeroMission] Error load hero mission. (msg:%s)", e);
        return 0;
    }
}

// ORIG 0x0815db98：GetOutBuffer<Taiwan::SigSaveHeroMissionData> null → throw
// "context_null"；GetDBHandle(9,0) null → throw "handle_null"；
// blob_to_str(0, sig+4, 0x100)；set_query("upDate event_hero_mission_data set
// mission='%s' where server_group=%d and charac_no=%u", blob,
// get_server_group(), sig->m_field0) exec 失败 throw "exec_update"。catch
// logFormat(0x413) 返 0。成功返 1。
bool DBSaveHeroMissionEventData::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigSaveHeroMissionData* sig =
            stream->GetOutBuffer<Taiwan::SigSaveHeroMissionData>();
        if (sig == 0)
            throw "context_null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        int serverGroup = G_CEnvironment()->get_server_group();
        unsigned int characNo = reinterpret_cast<SigHeroMissionDataView*>(sig)->m_field0;
        char* blob = db->blob_to_str(0, reinterpret_cast<SigHeroMissionDataView*>(sig)->m_data, 0x100);
        // ORIG 0x0815db98：set_query 返回值不检查（call 后直接 exec）；
        // 仅 exec(1) 失败才 throw "exec_update"。
        db->set_query(
            "upDate event_hero_mission_data set mission='%s' where server_group=%d and charac_no=%u",
            blob, serverGroup, characNo);
        if (!db->exec(1))
            throw "exec_update";
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x413,
            "[HeroMission] Error load hero mission. (msg:%s)", e);
        return 0;
    }
}
// ---- OnlyOrigBatch10：Taiwan DB 域 dispatch 实现 ----

// ORIG 0x0815b010 T LoadGeolocation::dispatch（报告 LoadGeolocation/dispatch.md）：
// GetDBHandle(1,0) null→throw "handle null"；get_server_group → set_query
// geo_allow_country → exec 失败 throw "exec"；逐行 get_str(0,buf,0x10) 失败 throw
// "get_str" → SigLoadGeolocation::set；count>0x2b 时 sendCountryData+init；末尾
// sendCountryData 一次。catch(const char*) logFormat 0x13c 返 0，成功返 1。
bool LoadGeolocation::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        int serverGroup = G_CEnvironment()->get_server_group();
        db->set_query(
            "seLect country_code from geo_allow_country where server_group = %d",
            serverGroup);
        if (!db->exec(true))
            throw "exec";
        char buf[0x10];
        memset(buf, 0, sizeof(buf));
        Taiwan::SigLoadGeolocation sig;
        sig.init();
        while (db->fetch())
        {
            memset(buf, 0, 0x10);
            if (db->get_str(0, buf, 0x10) != 1)
                throw "get_str";
            sig.set(buf);
            if (reinterpret_cast<int*>(&sig)[0] > 0x2b)
            {
                sendCountryData(sig);
                sig.init();
            }
        }
        sendCountryData(sig);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x13c,
            "[Taiwan, GeoIP] select geo_allow_country. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815b266 T LoadGeolocation::sendCountryData（报告 sendCountryData.md）：
// Acquire(...,0x144) → internal_stream(guard, 4, -1) → GetInBuffer<SigLoadGeolocation>
// null→throw "stream" → setData → put(Q1)。
void LoadGeolocation::sendCountryData(Taiwan::SigLoadGeolocation& data)
{
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x144), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)4, -1);
    Taiwan::SigLoadGeolocation* out =
        guard->GetInBuffer<Taiwan::SigLoadGeolocation>();   // ORIG via operator->
    if (out == 0)
        throw "stream";
    out->setData(data);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ORIG 0x0815a8f2 T LoadServiceRestrictPolicy::dispatch（报告 dispatch.md）：
// GetDBHandle(1,0) null→throw "handle null"；set_query 失败 throw "set_query"；
// exec 失败 throw "exec"；逐行 get_int(++idx,&v)/get_char(&c) 失败 throw
// "category"/"restrict"/"values"；sig.restrictValue==0 或 ==v1 时 sig.set(v2,v1,c)，
// 否则先 sendRestrictData+init 再 set；末尾 sendRestrictData。catch 0xd2 返 0。
bool LoadServiceRestrictPolicy::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        int serverGroup = G_CEnvironment()->get_server_group();
        if (!db->set_query(
                "seLect category, restrict_code, restrict_value from dnf_restrict_state where server_group=%d",
                serverGroup))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        Taiwan::SigLoadRestrictPolicy sig;
        sig.init();
        while (db->fetch())
        {
            int idx = 0;
            int v1 = 0;
            int v2 = 0;
            char c = 0;
            ++idx;
            if (db->get_int(idx, v1) != 1)
                throw "category";
            ++idx;
            if (db->get_int(idx, v2) != 1)
                throw "restrict";
            ++idx;
            if (db->get_char(idx, c) != 1)
                throw "values";
            if (sig.m_restrictValue == 0 || sig.m_restrictValue == v1)
            {
                sig.set(v2, v1, c);
            }
            else
            {
                sendRestrictData(sig);
                sig.init();
                sig.set(v2, v1, c);
            }
        }
        sendRestrictData(sig);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0xd2,
            "[Taiwan, Restrict] select dnf_restrict_state. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815ac40 T LoadServiceRestrictPolicy::sendRestrictData（报告
// sendRestrictData.md）：Acquire(...,0xd9) → internal_stream(guard, 1, -1) →
// GetInBuffer<SigLoadRestrictPolicy> null→throw "stream" → set → put(Q1)。
void LoadServiceRestrictPolicy::sendRestrictData(Taiwan::SigLoadRestrictPolicy& data)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0xd9), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)1, -1);
    Taiwan::SigLoadRestrictPolicy* out =
        guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
    if (out == 0)
        throw "stream";
    out->set(data);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ORIG 0x0815ad3c T SaveServiceRestrictPolicy::dispatch（报告 dispatch.md）：
// GetOutBuffer<SigLoadRestrictPolicy> null→throw "context null"；GetDBHandle(1,0)
// null→throw "handle null"；两循环（i=1..sig->m_count）：dnf_restrict_state
// values(%d,%d,%d,'%d')= (sg, restrictValue, i, m_extra[i])；dnf_restrict_info
// values(%d,%d,'%s')= (sg, restrictValue, i, &m_data[i*0x2d]+8)；set_query 失败
// throw "set_query" / exec 失败 throw "insert_exec"。catch 0x10e 返 0。
bool SaveServiceRestrictPolicy::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigLoadRestrictPolicy* sig =
            stream->GetOutBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        int serverGroup = G_CEnvironment()->get_server_group();
        for (int i = 1; i <= sig->m_count; ++i)
        {
            if (!db->set_query(
                    "inSert ignore into dnf_restrict_state (server_group , category , restrict_code , restrict_value , mod_date , reg_date) values(%d, %d, %d, '%d', now(), now())",
                    serverGroup, sig->m_restrictValue, i,
                    static_cast<int>(static_cast<signed char>(sig->m_extra[i]))))
                throw "set_query";
            if (!db->exec(true))
                throw "insert_exec";
        }
        for (int i = 1; i <= sig->m_count; ++i)
        {
            if (!db->set_query(
                    "inSert ignore into dnf_restrict_info (category , restrict_code , restrict_str , reg_date) values(%d, %d, '%s', now())",
                    sig->m_restrictValue, i,
                    sig->m_data[i]))
                throw "set_query";
            if (!db->exec(true))
                throw "insert_exec";
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x10e,
            "[Taiwan, Restrict] select dnf_restrict_state. (error:%s)", e);
        return 0;
    }
}

// ORIG 0x0815b76a T ResetStayTimeEvent::dispatch（报告 dispatch.md）：
// GetDBHandle(9,0) null→throw "handle null"；set_query("upDate
// event_stay_time_charac set reward_flag = 0")（不检查）→ exec 失败 throw "exec"。
// catch 0x1ac 返 0，成功返 1。
bool ResetStayTimeEvent::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        db->set_query("upDate event_stay_time_charac set reward_flag = 0");
        if (!db->exec(true))
            throw "exec";
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x1ac,
            "[Taiwan, Event] reset event_stay_time_charac. (error:%s)", e);
        return 0;
    }
}
