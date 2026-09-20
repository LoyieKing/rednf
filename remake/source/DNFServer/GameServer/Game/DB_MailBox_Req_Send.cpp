// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_MailBox_Req_Send/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
    unsigned int m_field04;         // +0x04（b28 P211：异型化 int 形参→值先载=ORIG）
    unsigned int m_field08;         // +0x08（同上；enum 形参异型）
    char m_pad0c[0x1e - 0x0c];      // +0x0c..+0x1d（senderName 余部）
    // +0x1e（S1 一次试验，P321 T1 直证=32 位读；ORIG InsertLetter 实参
    // `mov 0x1e(%eax),%eax; mov %eax,%edx` 两段中转、set_query vararg 单段
    // `mov 0x1e(%eax),%edx` ⇒ unsigned long 同时满足两处；int 恒单段）
    unsigned long m_senderCharacNo; // +0x1e
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

#include "DB_MailBox_Req_Send/sendResult.inc"


#include "DB_MailBox_Req_Send/InsertLetter.inc"


#include "DB_MailBox_Req_Send/insertMailData.inc"
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
        // b28d: P211 档③——LHS 与 RHS 异型（unsigned int）⇒ 4.4 出 ORIG 的
        // `mov t,%edx; imul; mov %edx,%ecx; sub %eax,%ecx; mov %ecx,%eax; mov %eax,t`
        // （原 (int) 同型形恒折 `sub %eax,-0x1c(%ebp)`，ORIG 0x0841f389..0x841f3a1）。
        t = (time_t)((unsigned int)t - ((const SigMailboxSendNewMailView*)sigRaw)->m_nB * 0x15180u);
    }
    else if (((const SigMailboxSendNewMailView*)sigRaw)->m_nB > 0xf)
    {
        // b28d: 同上——`(unsigned int)t` 触发 lea 合并形
        // （ORIG 0x0841f3bd..0x841f3ce `imul→%edx; mov t,%eax; lea (%edx,%eax,1),%eax`）。
        t = (time_t)((int)(((const SigMailboxSendNewMailView*)sigRaw)->m_nB * 0x15180u) + (unsigned int)t - 0x13c680);
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
        db->blob_to_str(0, (void*)&sigRaw->m_guid, 10),
        db->blob_to_str(1, (void*)&sigRaw->m_rand, 0xe),
        ((const SigMailboxSendNewMailView*)sigRaw)->m_upgradeSep,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_cashShopFlag);
    return db->exec(true);
}

#include "DB_MailBox_Req_Send/dispatch.inc"
