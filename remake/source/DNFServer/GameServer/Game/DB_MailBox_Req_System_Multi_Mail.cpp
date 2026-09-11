// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_MailBox_Req_System_Multi_Mail/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "DB_MailBox_Req_System_Multi_Mail.h"

#include "SigTypes.h"       // SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL / SIG_MAILBOX_SEND_NEW_MAIL
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr / s_db_mgr / s_systemTime_
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"      // cMyTrace
#include "Inven_Item.h"     // Inven_Item / stAmplifyOption_t / UpgradeSeparateInfo

#include <ctime>
#include <cstdio>
#include <string>

namespace WongWork
{
namespace DBCommon
{
unsigned int GetIdentity(MySQL* db);   // _ZN8WongWork8DBCommon11GetIdentityEP5MySQL
}
}

// 元素内 amp 双视图（stAmplifyOption_t 0xe 字节与 guid blob 复用同一 0x14
// 字节区；_InsertPostal 报告 `lea 0x7(%eax)`（getValues）/`add $0xb`（blob0））。
#pragma pack(push, 1)
// [M5 2026-09-11 · Main 批准] SigPostalItem130Amp / SigPostalItem 已上移至
// SigTypes_D.h（共享头具名化，pack(1)；原 TU-local 定义删除避免重复定义）。
// ORIG 基址换算（报告 _InsertPostal 0x0844372e 逐指令）：view A 基 = 元素-0x06
// （`add $0x120`）；view B 基 = 元素+0x0a（`add $0x130`）；seperate
// `add $0x150; add $0x9` = 元素+0x33；Inven_Item 方法 this = 元素基址；seal
// `add $0x126` 单段 = 元素+0x00。

// SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 字段镜像（pack(1)，sizeof 0x3a9；偏移对照
// dispatch/_InsertLetter/_InsertPostal 报告逐指令 + ReqDBSendNewSystemMultiMail）。
struct SigSystemMultiMailView
{
    char m_letterText[0x1e];       // +0x00 邮件正文（_InsertLetter escape 源）
    char m_sendCharacName[0x100];  // +0x1e 发送者名（_InsertLetter escape 源）
    int m_recvCharacNo;            // +0x11e（letter.charac_no / postal.receive_charac_no）
    int m_sendCharacNo;            // +0x122（letter.send_charac_no / postal.send_charac_no）
    SigPostalItem m_item[10];      // +0x126（10 × 0x3d = 0x262）
    unsigned int m_count;          // +0x388 附件数量（_InsertPostal 循环上界）
    int m_gold;                    // +0x38c 附带金币
    unsigned int m_duration;       // +0x390 时段数（dispatch 折算到期）
    unsigned char m_unlimitFlag;   // +0x394
    unsigned int m_serverGroup;    // +0x395 ENUM_SERVER_GROUP
    char m_pad399[4];              // +0x399..0x39c（无访问证据）
    int m_letterId;                // +0x39d DB 回填 letter_id（GetIdentity）
    int m_occTime;                 // +0x3a1 到期时间（dispatch 写入）
    unsigned int m_stat;           // +0x3a5 letter.stat（与 (int) 实参异型 ⇒ 4.4 出
                                   // ORIG 的 `mov 0x3a5(%eax),%eax; mov %eax,%ebx` 两段，
                                   // b28c-34 探针 /tmp/c28l34_probe3.cpp q 实证）
};
#pragma pack(pop)
static_assert(sizeof(SigPostalItem) == 0x3d, "postal item stride drift (ORIG imul $0x3d)");
static_assert(sizeof(SigSystemMultiMailView) == 0x3a9, "SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL mirror drift");
static_assert(offsetof(SigSystemMultiMailView, m_item) == 0x126, "items zone base drift");

// sendResult 回复包（SIG_MAILBOX_SEND_NEW_MAIL 前 3 个 int）
struct SigMailboxSendReplyView
{
    int m_field00;
    unsigned int m_field04;   // 与形参 userIdx(int) 异型（NL7 定律）：4.4 RHS 先载翻 ORIG 形
    int m_field08;
};

#include "DB_MailBox_Req_System_Multi_Mail/_InsertLetter.inc"


#include "DB_MailBox_Req_System_Multi_Mail/_InsertPostal.inc"


#include "DB_MailBox_Req_System_Multi_Mail/dispatch.inc"
{
    static const int handleIdx = 3;
    struct tm tmBuf;
    time_t now;
    SigSystemMultiMailView* sig = (SigSystemMultiMailView*)
        stream->GetOutBuffer<SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_3, (ENUM_SERVER_GROUP)sig->m_serverGroup);
    struct tm* tmPtr;
    time_t t;

    now = GlobalData::s_systemTime_.getCurSec();
    tmPtr = localtime_r(&now, &tmBuf);
    tmPtr->tm_hour += 1;
    tmPtr->tm_min = 0;
    tmPtr->tm_sec = 0;
    t = mktime(tmPtr);

    if (sig->m_duration < 0xf)
    {
        t = t - sig->m_duration * 0x15180;
    }
    else if (sig->m_duration > 0xf)
    {
        t = sig->m_duration * 0x15180 + t - 0x13c680;
    }
    sig->m_occTime = (int)t;

    if (!_InsertLetter(db, (SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL*)sig))
    {
        return 0;
    }
    if (!_InsertPostal(db, (SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL*)sig))
    {
        return 0;
    }
    sendResult(sig->m_recvCharacNo, (ENUM_SERVER_GROUP)sig->m_serverGroup);
    return 1;
}

#include "DB_MailBox_Req_System_Multi_Mail/sendResult.inc"
