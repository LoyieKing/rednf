// ============================================================================
// df_game_r 还原 —— DB_MailBox 域 DB 侧 dispatch 任务（OrigMailWave18 批，
// 独占 TU，未入 CMake，待 Main 登记）
// 覆盖 7 个 ORIG T 符号（地址见 DB_MailBox_Dispatch.h 头注释）：
//   1. getList2inQuery(uint*, char*)                    0x083ff968
//   2. DB_MailBox_Req_Delete::dispatch                 0x0841f084
//   3. DB_MailBox_Req_CheckReceiver::dispatch          0x0841c882
//   4. DB_Multi_MailBox_Req_Send::dispatch             0x0843bada
//   5. DB_Multi_MailBox_Req_Send::_sendResult          0x0843bc16
//   6. DB_ReqQueryCharacInfoMailbox::dispatch          0x0843bd46
//   7. DB_ReqQueryCharacInfoMailbox::_sendResult       0x0843c224
// 报告均已全文阅读后转写；SIG 布局：SIG_MAILBOX_DEL_MAIL / SIG_MAILBOX_LIST /
//   SIG_MAILBOX_CHECK_RECEIVER / SIG_QUERY_CHARAC_INFO 均为 SigTypes_D.h 的
//   pad/具名结构，本 TU 按 ORIG 偏移访问（跨字段区用裸偏移，语义照报告）。
// 依赖：DB_MailBox_Req_Send::InsertLetter（DB_MailBox_Req_Send.h，已实现）；
//   NumberToString 定义归 LinkFinalDeps.cpp；DBDispatcher::dispatch 归 DB_Login.cpp。
// ============================================================================

#include "DB_MailBox_Dispatch.h"

#include "SigTypes.h"       // SIG_MAILBOX_DEL_MAIL / SIG_MAILBOX_CHECK_RECEIVER / SIG_QUERY_CHARAC_INFO
#include "GlobalData.h"     // s_db_mgr / s_systemTime_ / s_stream_pool / s_big_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"      // cMyTrace
#include "LogManager.h"
#include "DNFFunctionLib.h" // NumberToString
#include "DB_MailBox_Req_Send.h"  // DB_MailBox_Req_Send::InsertLetter / ENUM_LETTER_STAT

#include <ctime>
#include <cstring>
#include <cstdio>

// ============================================================================
// 1. getList2inQuery —— ORIG 0x083ff968（_lt_global_gt_/getList2inQuery.md）：
//    out[0]=0 → memcpy(out,"in (",5) → for(i=1; i<list[0]; ++i)
//    sprintf(out,"%s%d,",out,list[i]) → sprintf(out,"%s%d)",out,list[i]) → out。
// ============================================================================
char* getList2inQuery(unsigned int* list, char* out)
{
    out[0] = '\0';
    memcpy(out, "in (", 5);
    unsigned int i;
    for (i = 1; i < list[0]; ++i)
    {
        sprintf(out, "%s%d,", out, list[i]);
    }
    sprintf(out, "%s%d)", out, list[i]);
    return out;
}

// ============================================================================
// 2. DB_MailBox_Req_Delete::dispatch —— ORIG 0x0841f084（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_DEL_MAIL> → GetDBHandle(DB_HANDLE_3,0)；
//    count==0 返 0；count==1 单条 upDate（postal_id=%d）；否则
//    getList2inQuery 生成列表串（postal_id %s）；exec(true) 失败返 0，成功返 1。
//    局部 static char strQuery[]（ORIG _ZZN21DB_MailBox_Req_Delete8dispatch
//    EiiP6StreamE8strQuery .bss 证据）。
// ============================================================================
bool DB_MailBox_Req_Delete::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_MAILBOX_DEL_MAIL* sig = stream->GetOutBuffer<SIG_MAILBOX_DEL_MAIL>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    if (*(int*)&sig->m_data[0] == 0)
        return 0;

    if (*(int*)&sig->m_data[0] == 1)
    {
        db->set_query(
            "upDate postal set delete_flag=%d,receive_time=now() where postal_id=%d",
            *(int*)&sig->m_data[0x54], *(int*)&sig->m_data[0x04]);
    }
    else
    {
        static char strQuery[0x100];
        db->set_query(
            "upDate postal set delete_flag=%d,receive_time=now() where postal_id %s",
            *(int*)&sig->m_data[0x54], getList2inQuery((unsigned int*)sig->m_data, strQuery));
    }

    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// 3. DB_MailBox_Req_CheckReceiver::dispatch —— ORIG 0x0841c882（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER> → 双 DB 句柄(2/3) →
//    charac_info 按收件人名查 charac_no/lev/delete_flag/m_id（循环读全部行）→
//    BigStreamPool 回包：<<0x7e <<slot，回填 +0x1d/+0x21/+0x25/+0x29，
//    memcpy(+0x31,0x3f88) 载荷透传 → 黑名单 count(*)>0 时 +0x3fb9=0x4d → put。
// ============================================================================
bool DB_MailBox_Req_CheckReceiver::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_MAILBOX_CHECK_RECEIVER* sig = stream->GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    int characNo = 0;
    int lev = 0;
    int deleteFlag = 0;
    unsigned int mId = 0;

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, (ENUM_SERVER_GROUP)0);
    MySQL* db3 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, (ENUM_SERVER_GROUP)0);

    char name[0x3c] = { '\0' };
    db2->escape_string(name, sig->m_receiverName);
    db2->set_query(
        "seLect charac_no,lev,delete_flag,m_id from charac_info where charac_name='%s'",
        name);
    if (!db2->exec(true))
        return 0;

    for (unsigned int i = 0; i < (unsigned int)db2->get_n_rows(); ++i)
    {
        if (!db2->fetch())
            return 0;
        if (!db2->get_int(0, characNo))
            return 0;
        if (!db2->get_int(1, lev))
            return 0;
        if (!db2->get_int(2, deleteFlag))
            return 0;
        if (!db2->get_uint(3, mId))
            return 0;
    }

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x509a), true);
    guard.operator*()->operator<<(0x7e);
    guard.operator*()->operator<<(slot);

    SIG_MAILBOX_CHECK_RECEIVER* out = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    *(unsigned int*)((char*)out + 0x1d) = (unsigned int)characNo;
    *(unsigned int*)((char*)out + 0x21) = (unsigned int)lev;
    *(unsigned int*)((char*)out + 0x25) = (unsigned int)deleteFlag;
    *(unsigned int*)((char*)out + 0x29) = mId;
    memcpy((char*)out + 0x31, (char*)sig + 0x31, 0x3f88);

    char* mIdStr = NumberToString(mId, 0);
    if (db3->set_query(
            "seLect count(*) from charac_black_list where m_id = %s and charac_no = %d",
            mIdStr, *(int*)((char*)sig + 0x2d)))
    {
        int blackCnt = 0;
        *(unsigned int*)((char*)out + 0x3fb9) = 0;
        if (db3->exec(true) && db3->fetch() && db3->get_int(0, blackCnt) && blackCnt > 0)
            *(unsigned int*)((char*)out + 0x3fb9) = 0x4d;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// 4. DB_Multi_MailBox_Req_Send::dispatch —— ORIG 0x0843bada（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER> → 到期时间 = 下一整点
//    （getCurSec+1h 清零分秒 → mktime）→ GetDBHandle(DB_HANDLE_3,0) →
//    InsertLetter(db, +0x1d 收件人, +0x2d 发件人, +0x3fc1 name, +0xdb title,
//    +0x3fbd letterId, t, 0)，失败 logFormat(0x8e53) → _sendResult(slot, sig)
//    → 返 1。
// ============================================================================
bool DB_Multi_MailBox_Req_Send::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_MAILBOX_CHECK_RECEIVER* sig = stream->GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER>();

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmBuf;
    struct tm* tmPtr = localtime_r(&now, &tmBuf);
    tmPtr->tm_hour += 1;
    tmPtr->tm_min = 0;
    tmPtr->tm_sec = 0;
    time_t t = mktime(tmPtr);

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    if (!DB_MailBox_Req_Send::InsertLetter(
            db, (int)sig->m_receiverCharacNo, (int)sig->m_senderCharacNo,
            (const char*)sig + 0x3fc1, (const char*)sig + 0xdb,
            *(int*)((char*)sig + 0x3fbd), t, LETTER_STAT_NONE))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x8e53,
                              "InsertLetter() error");
    }

    _sendResult(slot, sig);
    return 1;
}

// ============================================================================
// 5. DB_Multi_MailBox_Req_Send::_sendResult —— ORIG 0x0843bc16（_sendResult.md）：
//    BigStreamPool::Acquire("DBThread.cpp",0x8e67) → CStreamGuard(stream,true) →
//    <<0x1f5 → cMyTrace("bool ..._sendResult(int, SIG_MAILBOX_CHECK_RECEIVER*)",
//    0x8e6a,0)("E_MULTI_MAILBOX_REQ_SEND : %d",0x1f5) → <<slot →
//    GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER> → memcpy 0x3fde → put(DISPATCH_Q) → 1。
// ============================================================================
bool DB_Multi_MailBox_Req_Send::_sendResult(int slot, SIG_MAILBOX_CHECK_RECEIVER* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x8e67), true);

    guard.operator*()->operator<<(0x1f5);

    cMyTrace trace(__PRETTY_FUNCTION__, 0x8e6a, 0);
    trace("E_MULTI_MAILBOX_REQ_SEND : %d", 0x1f5);
    guard.operator*()->operator<<(slot);
    SIG_MAILBOX_CHECK_RECEIVER* out = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    memcpy(out, sig, 0x3fde);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
}

// ============================================================================
// 6. DB_ReqQueryCharacInfoMailbox::dispatch —— ORIG 0x0843bd46（dispatch.md 全文）：
//    基类 DBDispatcher::dispatch 失败返 0 → GetDBHandle(DB_HANDLE_2,0) →
//    GetOutBuffer<SIG_QUERY_CHARAC_INFO>（首字节即角色名）→ escape_string →
//    charac_info 查 7 列；exec 失败/无行/取行失败 → _sendResult(2,...)；
//    delete_flag!=0 → _sendResult(0x15,...)；成功 → 读列 1..6 →
//    _sendResult(0, slot, name, characNo, job, lev, growType, sex) → 返 1。
//    注意 ORIG 对 get_n_rows()==0 有两次独立判定（P48 双调用），照写。
// ============================================================================
bool DB_ReqQueryCharacInfoMailbox::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, (ENUM_SERVER_GROUP)0);
    SIG_QUERY_CHARAC_INFO* sig = stream->GetOutBuffer<SIG_QUERY_CHARAC_INFO>();

    char name[0x3c] = { '\0' };
    db->escape_string(name, (const char*)sig);
    db->set_query(
        "seLect delete_flag,m_id,charac_no,job,lev,grow_type,sex from charac_info where charac_name='%s'",
        name);
    if (!db->exec(true))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    if (db->get_n_rows() == 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    int deleteFlag = 0;
    if (!db->fetch() || !db->get_int(0, deleteFlag))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    if (deleteFlag != 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    int characNo = 0;
    int job = 0;
    int lev = 0;
    int growType = 0;
    int sex = 0;
    int mId = 0;
    db->get_int(1, mId);
    db->get_int(2, characNo);
    db->get_int(3, job);
    db->get_int(4, lev);
    db->get_int(5, growType);
    db->get_int(6, sex);

    _sendResult(0, slot, name, characNo, job, lev, growType, sex);
    return 1;
}

// ============================================================================
// 7. DB_ReqQueryCharacInfoMailbox::_sendResult —— ORIG 0x0843c224（_sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0x8ed6) → CStreamGuard(stream,true) →
//    <<0x1f6 → <<slot → GetInBuffer<SIG_QUERY_CHARAC_INFO> → +0x20=errCode；
//    errCode==0 时 strncpy(name,0x1d)、+0x24=characNo、+0x28=(char)job、
//    +0x2a=(short)lev、+0x2c=(char)growType、+0x2d=(char)sex → put → 返 1。
// ============================================================================
int DB_ReqQueryCharacInfoMailbox::_sendResult(int errCode, int slot, const char* name,
                                              int characNo, int job, int lev,
                                              int growType, int sex)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8ed6), true);

    guard.operator*()->operator<<(0x1f6);
    guard.operator*()->operator<<(slot);

    SIG_QUERY_CHARAC_INFO* out = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    *(int*)((char*)out + 0x20) = errCode;
    if (errCode == 0)
    {
        strncpy((char*)out, name, 0x1d);
        *(int*)((char*)out + 0x24) = characNo;
        *((char*)out + 0x28) = (char)job;
        *(short*)((char*)out + 0x2a) = (short)lev;
        *((char*)out + 0x2c) = (char)growType;
        *((char*)out + 0x2d) = (char)sex;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}