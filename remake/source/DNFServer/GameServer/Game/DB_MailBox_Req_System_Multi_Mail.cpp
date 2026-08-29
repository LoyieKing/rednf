// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_System_Multi_Mail（OrigSysMail 批，4 ORIG 符号）
// 报告：docs/class_func_reports/DB_MailBox_Req_System_Multi_Mail/{dispatch,
//   _InsertLetter,_InsertPostal,sendResult}.md（逐指令转写；objdump 已核实地址）
// 依赖：
//   - SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL（SigTypes_D.h，m_data[0x3a9] pad）——
//     本 TU 以 pack(1) 镜像 SigSystemMultiMailView 按 ORIG 偏移访问。
//   - _InsertPostal 的元素域（stride 0x3d）：ORIG 以三套寻址访问——
//     +0x120 结构（item_id/add_info/endurance/extend）、+0x130 结构
//     （amp@+0x7/blob0@+0xb/blob1@+0x1b/seperate@+0x29）、
//     seal 用 `sigRaw+i*0x3d+0x126` 单段、Inven_Item 方法用
//     `sigRaw+i*0x3d+0x120+0x6` 两段。逐指令形态见报告。
//   - WongWork::DBCommon::GetIdentity 定义归 WongWorkEvents_A.cpp（TU 局部声明同款）。
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

// SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 字段镜像（pack(1)，sizeof 0x3a9；偏移对照
// dispatch/_InsertLetter/_InsertPostal 报告逐指令 + ReqDBSendNewSystemMultiMail）。
#pragma pack(push, 1)
struct SigSystemMultiMailView
{
    char m_letterText[0x1e];       // +0x00 邮件正文（_InsertLetter escape 源）
    char m_sendCharacName[0x100];  // +0x1e 发送者名（_InsertLetter escape 源）
    int m_sendCharacNo;            // +0x11e
    int m_recvCharacNo;            // +0x122
    char m_items[0x262];           // +0x126（m_count 个 0x3d 字节元素）
    unsigned int m_count;          // +0x388 附件数量（_InsertPostal 循环上界）
    int m_gold;                    // +0x38c 附带金币
    unsigned int m_duration;       // +0x390 时段数（dispatch 折算到期）
    unsigned char m_unlimitFlag;   // +0x394
    unsigned int m_serverGroup;    // +0x395 ENUM_SERVER_GROUP
    char m_pad399[4];              // +0x399..0x39c（无访问证据）
    int m_letterId;                // +0x39d DB 回填 letter_id（GetIdentity）
    int m_occTime;                 // +0x3a1 到期时间（dispatch 写入）
    int m_stat;                    // +0x3a5 letter.stat
};
#pragma pack(pop)

// 元素域视图 A（基址 sig+0x120）：item_id/add_info/endurance/extend
#pragma pack(push, 1)
struct SigPostalItem120
{
    char m_pad0[0x6];              // +0x00（0x120..0x126）
    unsigned char m_sealFlag;      // +0x06（0x126）
    char m_pad7;                   // +0x07
    int m_itemId;                  // +0x08
    char m_padC;                   // +0x0c
    int m_addInfo;                 // +0x0d
    unsigned short m_endurance;    // +0x11
    int m_extendInfo;              // +0x13
    char m_rest[0x3d - 0x17];      // +0x17..+0x3d
};
union SigPostalItem130Amp
{
    stAmplifyOption_t m_amp;   // +0x07（0x137，0xe 字节）
    struct
    {
        char m_ampHead[0x4];   // +0x07
        char m_guidBlob[0x10]; // +0x0b（blob_to_str(0) 源，10 字节）
    } m_g;
};

// 元素域视图 B（基址 sig+0x130）：amp / blob 区 / seperate_upgrade
#pragma pack(push, 1)
struct SigPostalItem130
{
    char m_pad0[0x7];              // +0x00（0x130..0x137）
    SigPostalItem130Amp m_ag;      // +0x07（0x137）
    char m_randomBlob[0xe];        // +0x1b（blob_to_str(1) 源，0xe 字节）
    unsigned char m_seperateUpgrade;  // +0x29（0x159）
    char m_rest[0x3d - 0x2a];      // +0x2a..+0x3d
};
#pragma pack(pop)

// sendResult 回复包（SIG_MAILBOX_SEND_NEW_MAIL 前 3 个 int）
struct SigMailboxSendReplyView
{
    int m_field00;
    int m_field04;
    int m_field08;
};

// ============================================================================
// _InsertLetter —— ORIG 0x084435d4：escape(+0x1e 名)→0x1ff 缓冲、escape(+0x00
// 正文)→0x3d 缓冲 → set_query(letter insert 6 列) → exec → GetIdentity 回填
// +0x39d → 1。
// ============================================================================
bool DB_MailBox_Req_System_Multi_Mail::_InsertLetter(
    MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sigRaw)
{
    char titleBuf[0x1ff] = { 0 };
    db->escape_string(titleBuf,
                      ((SigSystemMultiMailView*)sigRaw)->m_sendCharacName);

    char nameBuf[0x3d] = { 0 };
    db->escape_string(nameBuf, ((SigSystemMultiMailView*)sigRaw)->m_letterText);

    db->set_query(
        "inSert into letter(charac_no,send_charac_no,send_charac_name,letter_text,reg_date,stat) values(%d,%d,'%s','%s',from_unixtime(%d),%d)",
        ((SigSystemMultiMailView*)sigRaw)->m_sendCharacNo,
        ((SigSystemMultiMailView*)sigRaw)->m_recvCharacNo, nameBuf, titleBuf,
        ((SigSystemMultiMailView*)sigRaw)->m_occTime,
        (int)((SigSystemMultiMailView*)sigRaw)->m_stat);

    if (!db->exec(true))
    {
        return 0;
    }
    ((SigSystemMultiMailView*)sigRaw)->m_letterId =
        (int)WongWork::DBCommon::GetIdentity(db);
    return 1;
}

// ============================================================================
// _InsertPostal —— ORIG 0x0844372e：std::string 拼 "(...)" 20 列（循环 count
// 个附件，i!=0 时 gold 清 0；每项 getValues/GetUpgradeSeparate/blob_to_str 等）
// → set_query(postal insert ... values %s) → exec 失败 cMyTrace → 1。
// ============================================================================
bool DB_MailBox_Req_System_Multi_Mail::_InsertPostal(
    MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sigRaw)
{
    unsigned short ampValue;
    unsigned char ampType;
    std::string str;
    unsigned int i;

    str.clear();

    for (i = 0; i < ((SigSystemMultiMailView*)sigRaw)->m_count; ++i)
    {
        char buf[0x400] = { 0 };

        if (i != 0)
        {
            ((SigSystemMultiMailView*)sigRaw)->m_gold = 0;
        }

        ampType = 0;
        ampValue = 0;
        ((SigPostalItem130*)((char*)sigRaw + 0x130))[i].m_ag.m_amp.getValues(
            ampType, ampValue);

        sprintf(buf,
            "(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s',%d,%d)",
            ((SigSystemMultiMailView*)sigRaw)->m_occTime,
            ((SigSystemMultiMailView*)sigRaw)->m_sendCharacNo,
            ((SigSystemMultiMailView*)sigRaw)->m_recvCharacNo,
            *(unsigned char*)((char*)sigRaw + i * 0x3d + 0x126),
            ((SigPostalItem120*)((char*)sigRaw + 0x120))[i].m_itemId,
            ((SigPostalItem120*)((char*)sigRaw + 0x120))[i].m_addInfo,
            ((SigPostalItem120*)((char*)sigRaw + 0x120))[i].m_endurance,
            ((Inven_Item*)((char*)sigRaw + i * 0x3d + 0x120 + 0x6))->GetItemAttr(),
            ampType, ampValue,
            ((SigSystemMultiMailView*)sigRaw)->m_gold, (const char*)sigRaw,
            ((Inven_Item*)((char*)sigRaw + i * 0x3d + 0x120 + 0x6))->IsCreatureItemType(),
            ((Inven_Item*)((char*)sigRaw + i * 0x3d + 0x120 + 0x6))->isAvatarItemType(),
            ((SigSystemMultiMailView*)sigRaw)->m_letterId,
            ((SigPostalItem120*)((char*)sigRaw + 0x120))[i].m_extendInfo,
            db->blob_to_str(0,
                (void*)&((SigPostalItem130*)((char*)sigRaw + 0x130))[i].m_ag.m_g.m_guidBlob[0], 10),
            db->blob_to_str(1,
                (void*)&((SigPostalItem130*)((char*)sigRaw + 0x130))[i].m_randomBlob[0], 0xe),
            ((UpgradeSeparateInfo*)&((SigPostalItem130*)((char*)sigRaw + 0x130))[i].m_seperateUpgrade)->GetUpgradeSeparate(),
            ((SigSystemMultiMailView*)sigRaw)->m_unlimitFlag);
        str.append(buf);

        if (i < ((SigSystemMultiMailView*)sigRaw)->m_count - 1)
        {
            str.append(",");
        }
    }

    db->set_query(
        "inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade ,amplify_option,amplify_value,gold,send_charac_name,creature_flag,avata_flag,letter_id,extend_info,item_guid,random_option,seperate_upgrade,unlimit_flag) values %s",
        str.c_str());

    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa5b5, 5)(
            "Insert Postal Fail - inQuery(%s)", str.c_str());
        return 0;
    }
    return 1;
}

// ============================================================================
// dispatch —— ORIG 0x0844346a（虚覆盖）：GetOutBuffer<SIG_MAILBOX_SEND_SYSTEM_
// MULTI_MAIL> → GetDBHandle(DB_HANDLE_3, +0x395) → getCurSec → localtime_r
// (+1h/min/sec=0) → mktime → 按 +0x390 时段折算 → 写 +0x3a1 → _InsertLetter →
// _InsertPostal（失败返 0）→ sendResult(+0x11e, +0x395) → 1。
// ============================================================================
bool DB_MailBox_Req_System_Multi_Mail::dispatch(int userIdx, int slot, Stream* stream)
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
    sendResult(sig->m_sendCharacNo, (ENUM_SERVER_GROUP)sig->m_serverGroup);
    return 1;
}

// ============================================================================
// sendResult —— ORIG 0x08443b6e：Acquire("DBThread.cpp",0xa5be) → CStreamGuard
// (stream,true) → <<0x80(int) → <<-1(int) → GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>
// → 写 +0x00=0/+0x04=userIdx/+0x08=group → put(DISPATCH_Q, guard) → 1。
// ============================================================================
int DB_MailBox_Req_System_Multi_Mail::sendResult(int userIdx,
                                                  ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa5be), true);

    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<(-1);

    SigMailboxSendReplyView* sig = (SigMailboxSendReplyView*)
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    sig->m_field00 = 0;
    sig->m_field04 = userIdx;
    sig->m_field08 = (int)group;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}