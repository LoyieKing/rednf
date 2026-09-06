// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_Send（OrigMailSend 批，4 ORIG 符号）
// 报告：docs/class_func_reports/DB_MailBox_Req_Send/{InsertLetter,insertMailData,
//   sendResult,dispatch}.md（逐指令转写；objdump 已核实地址）
// 依赖：SIG_MAILBOX_SEND_NEW_MAIL（SigTypes_D.h，m_data[0x168] pad）——本 TU 以
//   pack(1) 镜像 SigMailboxSendNewMailView 按 ORIG 偏移访问（CMailBoxHelper.cpp
//   MailBoxSendNewMailV 同源，0x8555756 _makeSendMailData 逐字段证据）。
// WongWork::DBCommon::GetIdentity 定义归 WongWorkEvents_A.cpp（TU 局部声明同款）。
// ============================================================================

#include "DB_MailBox_Req_Send.h"

#include "SigTypes.h"       // SIG_MAILBOX_SEND_NEW_MAIL
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr / s_db_mgr / s_systemTime_
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"

#include <ctime>
#include <cstring>

namespace WongWork
{
namespace DBCommon
{
unsigned int GetIdentity(MySQL* db);   // _ZN8WongWork8DBCommon11GetIdentityEP5MySQL
}
}

// SIG_MAILBOX_SEND_NEW_MAIL 字段镜像（pack(1)，sizeof 0x168；偏移对照
// CMailBoxHelper.cpp MailBoxSendNewMailV + insertMailData 报告逐指令）。
#pragma pack(push, 1)
struct SigMailboxSendNewMailView
{
    int m_field00;                  // +0x00（sendResult 写 0；insertMailData 视为 senderName 起始）
    int m_field04;                  // +0x04
    int m_field08;                  // +0x08
    char m_pad0c[0x1e - 0x0c];      // +0x0c..+0x1d（senderName 余部）
    int m_senderCharacNo;           // +0x1e
    unsigned int m_amount;          // +0x22
    unsigned int m_destCharacNo;    // +0x26
    unsigned char m_itemField0;     // +0x2a
    int m_addInfo;                  // +0x2b
    int m_addInfo2;                 // +0x2f
    unsigned char m_itemAttr;       // +0x33
    unsigned short m_fieldb;        // +0x34
    int m_fieldd;                   // +0x36
    int m_field15;                  // +0x3a
    int m_field19;                  // +0x3e
    unsigned short m_field1d;       // +0x42
    unsigned char m_ampType;        // +0x44
    unsigned short m_ampIdx;        // +0x45
    int m_field25;                  // +0x47
    int m_field29;                  // +0x4b
    int m_field2d;                  // +0x4f
    unsigned short m_field31;       // +0x53
    unsigned char m_upgradeSep;     // +0x55
    unsigned char m_isAvatarSend;   // +0x56
    unsigned char m_cashShopFlag;   // +0x57
    unsigned char m_isCreature;     // +0x58
    unsigned short m_titleLen;      // +0x59
    char m_title[0x100];            // +0x5b
    unsigned int m_nB;              // +0x15b
    unsigned int m_serverGroup;     // +0x15f
    int m_nType;                    // +0x163
    unsigned char m_flag;           // +0x167
};
#pragma pack(pop)

// ============================================================================
// sendResult —— ORIG 0x0841f75a：Acquire("DBThread.cpp",0x559f) → CStreamGuard
// (stream,true) → <<0x80(int) → <<-1(int) → GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>
// → 写 +0x00=0/+0x04=param_3/+0x08=param_4 → put(DISPATCH_Q, guard) → return 1。
// ============================================================================
int DB_MailBox_Req_Send::sendResult(int userIdx, int slot, int mailIdx,
                                    ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x559f), true);

    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<(-1);

    SigMailboxSendNewMailView* sig = reinterpret_cast<SigMailboxSendNewMailView*>(
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>());
    sig->m_field00 = 0;
    sig->m_field04 = mailIdx;
    sig->m_field08 = (int)group;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// InsertLetter —— ORIG 0x0841f15a：strlen(title) > 0xaa → logFormat(0x54bc) 返 0；
// escape title→0x1ff 缓冲、name→0x3d 缓冲 → set_query(letter insert) →
// exec → GetIdentity → *letterId → 1。
// ============================================================================
bool DB_MailBox_Req_Send::InsertLetter(MySQL* db, int characNo, int sendCharacNo,
                                       const char* name, const char* title,
                                       int& letterId, time_t regDate,
                                       ENUM_LETTER_STAT stat)
{
    if (strlen(title) > 0xaa)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x54bc,
            "send_charac_no : %d, recv_charac_no : %d, letter_size : %d",
            sendCharacNo, characNo, (int)strlen(title));
        return 0;
    }

    char titleBuf[0x1ff] = { 0 };
    db->escape_string(titleBuf, title);

    char nameBuf[0x3d] = { 0 };
    db->escape_string(nameBuf, name);

    db->set_query(
        "inSert into letter(charac_no,send_charac_no,send_charac_name,letter_text,reg_date,stat) values(%d,%d,'%s','%s',from_unixtime(%d),%d)",
        characNo, sendCharacNo, nameBuf, titleBuf, (long)regDate, (int)stat);

    if (!db->exec(true))
    {
        return 0;
    }
    letterId = (int)WongWork::DBCommon::GetIdentity(db);
    return 1;
}

// ============================================================================
// insertMailData —— ORIG 0x0841f2fa（static bool）：
//   getCurSec → localtime_r(+1h,min/sec=0) → mktime → 按 m_flag/m_nB 折算到期
//   → m_nType!=0 且 !=last_letter_id 时 upDate letter → last_letter_id 缓存
//   → m_titleLen!=0 且 letterId==0 时 InsertLetter（失败 logFormat 0x550c），
//     若 m_addInfo==0 && m_destCharacNo==0 直接返 1
//   → 拷贝 senderName → blob_to_str(0,+0x3a,10)/(1,+0x47,0xe) →
//     set_query(postal insert 20 列) → exec。
// ============================================================================
bool DB_MailBox_Req_Send::insertMailData(MySQL* db, const SIG_MAILBOX_SEND_NEW_MAIL* sigRaw)
{
    static int last_letter_id = 0;   // ORIG _ZZN19DB_MailBox_Req_Send14insertMailDataEP5MySQLPK25SIG_MAILBOX_SEND_NEW_MAILE14last_letter_id 0x0943c230
    struct tm tmBuf;
    time_t now;
    struct tm* tmPtr;
    time_t t;
    int letterId;

    now = GlobalData::s_systemTime_.getCurSec();
    tmPtr = localtime_r(&now, &tmBuf);
    tmPtr->tm_hour += 1;
    tmPtr->tm_min = 0;
    tmPtr->tm_sec = 0;
    t = mktime(tmPtr);

    if (((const SigMailboxSendNewMailView*)sigRaw)->m_flag != 0)
    {
        t = (int)((const SigMailboxSendNewMailView*)sigRaw)->m_nB - 0x13c680;
    }
    else if (((const SigMailboxSendNewMailView*)sigRaw)->m_nB <= 0xe)
    {
        t = (time_t)((int)t - (int)(((const SigMailboxSendNewMailView*)sigRaw)->m_nB * 0x15180u));
    }
    else if (((const SigMailboxSendNewMailView*)sigRaw)->m_nB > 0xf)
    {
        t = (time_t)((int)(((const SigMailboxSendNewMailView*)sigRaw)->m_nB * 0x15180u) + (int)t - 0x13c680);
    }

    if (((const SigMailboxSendNewMailView*)sigRaw)->m_nType != 0 &&
        ((const SigMailboxSendNewMailView*)sigRaw)->m_nType != last_letter_id)
    {
        db->set_query("upDate letter set stat =%d where letter_id = %d",
                      1, ((const SigMailboxSendNewMailView*)sigRaw)->m_nType);
        db->exec(true);
    }
    last_letter_id = ((const SigMailboxSendNewMailView*)sigRaw)->m_nType;
    letterId = ((const SigMailboxSendNewMailView*)sigRaw)->m_nType;

    if (((const SigMailboxSendNewMailView*)sigRaw)->m_titleLen != 0 && letterId == 0)
    {
        if (!InsertLetter(db, ((const SigMailboxSendNewMailView*)sigRaw)->m_amount,
                          ((const SigMailboxSendNewMailView*)sigRaw)->m_senderCharacNo,
                          (const char*)sigRaw, ((const SigMailboxSendNewMailView*)sigRaw)->m_title,
                          letterId, t, (ENUM_LETTER_STAT)1))
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x550c,
                                  "InsertLetter() error");
        }
        if (((const SigMailboxSendNewMailView*)sigRaw)->m_addInfo == 0 &&
            ((const SigMailboxSendNewMailView*)sigRaw)->m_destCharacNo == 0)
        {
            return 1;
        }
    }

    char nameBuf[0x57] = { 0 };
    strcpy(nameBuf, (const char*)sigRaw);
    db->set_query(
        "inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade,amplify_option,amplify_value,gold,send_charac_name,creature_flag,avata_flag,letter_id,extend_info,item_guid,random_option,seperate_upgrade,unlimit_flag) values(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s',%d,%d)",
        t, ((const SigMailboxSendNewMailView*)sigRaw)->m_senderCharacNo,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_amount,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_itemField0,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_addInfo,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_addInfo2,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_fieldb,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_itemAttr,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_ampType,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_ampIdx,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_destCharacNo, nameBuf,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_isCreature,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_isAvatarSend, letterId,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_fieldd,
        // blob_to_str 按 ORIG +0x3a/+0x47 裸偏移（byte-blob 读 10/0xe 字节，非自然宽度
        // 字段；&view->m_field15/25 的 int 类型与读长不配会产生地址物化差异）。
        db->blob_to_str(0, (void*)((char*)sigRaw + 0x3a), 10),
        db->blob_to_str(1, (void*)((char*)sigRaw + 0x47), 0xe),
        ((const SigMailboxSendNewMailView*)sigRaw)->m_upgradeSep,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_cashShopFlag);
    return db->exec(true);
}

// ============================================================================
// dispatch —— ORIG 0x0841f84e（虚覆盖）：GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL>
// → GetDBHandle(DB_HANDLE_3, sig+0x15f) → insertMailData 失败返 0；
// 成功 sendResult(slot, 0, sig+0x22, sig+0x15f)。
// ============================================================================
bool DB_MailBox_Req_Send::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_MAILBOX_SEND_NEW_MAIL* sigRaw = stream->GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_3, (ENUM_SERVER_GROUP)((const SigMailboxSendNewMailView*)sigRaw)->m_serverGroup);
    if (!insertMailData(db, sigRaw))
    {
        return 0;
    }
    return sendResult(slot, 0,
                      (int)((const SigMailboxSendNewMailView*)sigRaw)->m_amount,
                      (ENUM_SERVER_GROUP)((const SigMailboxSendNewMailView*)sigRaw)->m_serverGroup);
}