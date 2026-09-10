// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OrigDBInter20/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "SigTypes.h"       // SIG_SAVE_CONDITION_EVENT / SIG_SECURITY_CARD_UPDATE_FAIL_CNT /
                            // SIG_SECURITY_CARD_UPDATE_VALIDITYTIME / SIG_SECURITY_CARD_ADD_CERT_CNT
#include "GlobalData.h"     // s_db_mgr / s_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "DBDispatcher.h"
#include "DBMgr.h"          // DBMgr::GetDBHandle
#include "MySQL.h"          // MySQL::set_query / exec / getAffectedRowCount
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP
// NumberToString(unsigned int, int)（ORIG T 0x0810904b，LinkFinalDeps.cpp 定义）
extern char* NumberToString(unsigned int value, int radix);

#include <string.h>         // strncpy

// get_rand_int 自由函数（ORIG T 0x086b1b87，DNFFunctionLib.cpp 实现）
#include "OrigDBInter20/get_rand_int.inc"
// generateSecurityCardQuestion 自由函数（ORIG T 0x086b216d，本文件尾部定义）
#include "OrigDBInter20/generateSecurityCardQuestion.inc"

// ============================================================================
// 1. DB_SaveConditionEventInfo::dispatch —— ORIG 0x08437930（dispatch.md）：
//    GetDBHandle(2,0) → GetOutBuffer<SIG_SAVE_CONDITION_EVENT>（+0 uint charac_no /
//    +4 short current_step / +6 short reward_step）→ set_query(upDate ...) →
//    exec(true) 失败返 0；getAffectedRowCount()==0 时 set_query(inSert ...) →
//    exec(true) 失败返 0；返 1。字段访问经 SIG 直读（ORIG movzwl/movswl/cwtl
//    符号扩展形态，P9 语义）。
// ============================================================================
class DB_SaveConditionEventInfo : public DBDispatcher
{
public:
    DB_SaveConditionEventInfo() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#include "OrigDBInter20/dispatch.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_CONDITION_EVENT* sig = stream->GetOutBuffer<SIG_SAVE_CONDITION_EVENT>();

    db->set_query(
        "upDate event_conditionable_info set current_step=%d, reward_step=%d, update_time=now() where charac_no=%u",
        (int)sig->m_a2, (int)sig->m_a3, sig->m_value);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_conditionable_info(charac_no, current_step, reward_step, update_time) values(%u, %d, %d, now())",
            sig->m_value, (int)sig->m_a2, (int)sig->m_a3);
        if (!db->exec(true))
        {
            return 0;
        }
    }
    return 1;
}

class DB_SecurityCardUpdateCancelCnt : public DBDispatcher
{
public:
    DB_SecurityCardUpdateCancelCnt() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_UPDATE_FAIL_CNT 的 +4 cancel_cnt 字段（ORIG
// 单指令 disp 折叠寻址 mov 0x4(%edx),%edx）。
struct SecurityCardCancelCntView
{
    unsigned int m_mId;      // +0x00
    unsigned int m_cancelCnt;// +0x04
};

#include "OrigDBInter20/dispatch_1.inc"
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();

    db->set_query(
        "upDate member_security_card set cancel_cnt=%d where m_id=%s and apply_flag=1",
        reinterpret_cast<SecurityCardCancelCntView*>(sig)->m_cancelCnt,
        NumberToString(
            reinterpret_cast<SecurityCardCancelCntView*>(sig)->m_mId, 0));
    db->exec(true);
    return 1;
}

// ============================================================================
// 3. DB_SecurityCardUpdateFailCnt::dispatch —— ORIG 0x0842bdda（dispatch.md）：
//    GetDBHandle(6,0) → GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>；
//    +4 failCnt==0 时 generateSecurityCardQuestion(buf[5]) 并带 q_pos='%s'，
//    否则仅 fail_cnt=%d；均 NumberToString(m_id,0)；exec(true) →
class DB_SecurityCardUpdateFailCnt : public DBDispatcher
{
public:
    DB_SecurityCardUpdateFailCnt() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_UPDATE_FAIL_CNT 的 +4 fail_cnt 字段。
struct SecurityCardFailCntView
{
    unsigned int m_mId;   // +0x00
    unsigned int m_failCnt;// +0x04
};

#include "OrigDBInter20/dispatch_2.inc"
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();

    // ORIG（P48 无 CSE）：sig 字段每次即时重读，不缓存局部。
    if (reinterpret_cast<SecurityCardFailCntView*>(sig)->m_failCnt == 0)
    {
        char buf[5] = { 0 };
        generateSecurityCardQuestion(buf);
        db->set_query(
            "upDate member_security_card set fail_cnt=%d , q_pos = '%s' where m_id=%s and apply_flag=1",
            reinterpret_cast<SecurityCardFailCntView*>(sig)->m_failCnt,
            buf,
            NumberToString(
                reinterpret_cast<SecurityCardFailCntView*>(sig)->m_mId, 0));
    }
    else
    {
        db->set_query(
            "upDate member_security_card set fail_cnt=%d where m_id=%s and apply_flag=1",
            reinterpret_cast<SecurityCardFailCntView*>(sig)->m_failCnt,
            NumberToString(
                reinterpret_cast<SecurityCardFailCntView*>(sig)->m_mId, 0));
    }
    db->exec(true);

    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set security_card_fail_cnt=%d where m_id=%s",
        reinterpret_cast<SecurityCardFailCntView*>(sig)->m_failCnt,
        NumberToString(
            reinterpret_cast<SecurityCardFailCntView*>(sig)->m_mId, 0));
    if (!dbGrade->exec(true) || dbGrade->getAffectedRowCount() == 0)
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, security_card_fail_cnt) values(%s, %d)",
            NumberToString(
                reinterpret_cast<SecurityCardFailCntView*>(sig)->m_mId, 0),
            reinterpret_cast<SecurityCardFailCntView*>(sig)->m_failCnt);
        if (!dbGrade->exec(true))
        {
            return 0;
        }
    }
    return 1;
}

class DB_SecurityCardUpdateValidityTime : public DBDispatcher
{
public:
    DB_SecurityCardUpdateValidityTime() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_UPDATE_VALIDITYTIME 的 +4 flag 字段。
struct SecurityCardValidityTimeView
{
    unsigned int m_mId;   // +0x00
    unsigned int m_flag;  // +0x04
};

#include "OrigDBInter20/dispatch_3.inc"
                                                 Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_VALIDITYTIME* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_VALIDITYTIME>();

    db->set_query(
        "upDate member_security_card set validity_time=unix_timestamp(now())+%d where m_id=%s and apply_flag=1",
        reinterpret_cast<SecurityCardValidityTimeView*>(sig)->m_flag,
        NumberToString(
            reinterpret_cast<SecurityCardValidityTimeView*>(sig)->m_mId, 0));
    db->exec(true);
    return 1;
}

class DB_SecurityCardAddCertCnt : public DBDispatcher
{
public:
    DB_SecurityCardAddCertCnt() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_ADD_CERT_CNT 的 +0 m_id 字段。
struct SecurityCardAddCertCntView
{
    unsigned int m_mId;   // +0x00
};

#include "OrigDBInter20/dispatch_4.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_ADD_CERT_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_ADD_CERT_CNT>();

    db->set_query(
        "upDate member_security_card set cert_cnt = cert_cnt+1  where m_id=%s and apply_flag=1",
        NumberToString(
            reinterpret_cast<SecurityCardAddCertCntView*>(sig)->m_mId, 0));
    db->exec(true);
    return 1;
}

// ============================================================================
// 6. generateSecurityCardQuestion(char* out) —— ORIG 0x086b216d（UpdateFailCnt
//    dispatch 依赖，ORIG T / NEW 无；直接反汇编转写）：
//    栈槽（P28 声明序）：ORIG v1(-0x18)/a1(-0x14)/v2(-0x10)/a2(-0xc)。
// ============================================================================
#include "OrigDBInter20/generateSecurityCardQuestion_1.inc"
{
    int v1, a1, v2, a2;   // P28 声明序 → 槽 -0x18/-0x14/-0x10/-0xc
    a1 = get_rand_int(4);
    if (a1 == 3)
    {
        v1 = get_rand_int(8);
    }
    else
    {
        v1 = get_rand_int(10);
    }
    do
    {
        a2 = get_rand_int(4);
        if (a2 == 3)
        {
            v2 = get_rand_int(8);
        }
        else
        {
            v2 = get_rand_int(10);
        }
    } while ((v1 / 2 == v2 / 2) && (a1 == a2));

    out[0] = static_cast<char>(v1 + 0x30);
    out[1] = static_cast<char>(a1 + 0x30);
    out[2] = static_cast<char>(v2 + 0x30);
    out[3] = static_cast<char>(a2 + 0x30);
}
