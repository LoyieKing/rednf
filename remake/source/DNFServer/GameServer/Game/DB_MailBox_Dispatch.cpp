// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_MailBox_Dispatch/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "DB_MailBox_Dispatch/dispatch.inc"


#include "DB_MailBox_Dispatch/dispatch_1.inc"


#include "DB_MailBox_Dispatch/dispatch_2.inc"
{
    // b28d-SweepDBMailBoxDispatch（AE，84=84，jcc 全等）：title 实参已改整型加法形
    // 复现 ORIG 两步 add/mov（见调用点注释）。
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
            sig->m_senderName,                 // +0x3fc1（0843bb6e lea）
            // 报告 +0xdb：透传区内标题串（区无字段化证据）。等价于 m_payload+0xaa
            // （m_payload@+0x31）；P319 整型加法形复现 ORIG 0x0843bb71 两步
            // `add $0xdb,%eax; mov %eax,%ecx`（数组退化在 4.4 恒折单条 lea）。
            sig->m_title,
            sig->m_letterId,                   // +0x3fbd int&（0843bb65 lea；InsertLetter Ri）
            t, LETTER_STAT_NONE))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x8e53,
                              "InsertLetter() error");
    }

    _sendResult(slot, sig);
    return 1;
}

#include "DB_MailBox_Dispatch/_sendResult.inc"


#include "DB_MailBox_Dispatch/dispatch_3.inc"


#include "DB_MailBox_Dispatch/_sendResult_1.inc"
