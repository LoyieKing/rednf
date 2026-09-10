// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OrigDBInter19/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "SigTypes.h"       // SIG_GET_USER_REGDATE / SIG_QUERY_CHARAC_INFO /
                            // SIG_REQUEST_TAG_CHARAC_INFO / SIG_SECURITY_CARD_ISSUE /
                            // SIG_SECURITY_CARD_RETRANSFER
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "DBDispatcher.h"

#include <cstring>          // strncpy / memcpy

// ENUM_ERROR 权威定义 CUser.h（DNF_GAME_ENUM_ERROR_DEFINED 守卫，幂等复制见
// advancealtar.h 先例；本 TU 不反向 include CUser.h 避免重入环）。
#ifndef DNF_GAME_ENUM_ERROR_DEFINED
#define DNF_GAME_ENUM_ERROR_DEFINED
enum ENUM_ERROR { ENUM_ERROR_NONE = 0 };
#endif

namespace {

// SIG_SECURITY_CARD_ISSUE 载荷视图（ORIG _sendResult @0x842b410 写偏移实证）：
// +0x04 strncpy 0x21 / +0x25 strncpy 0xc / +0x37 strncpy 0x27 / +0x5e strncpy 5 /
// +0x8c ENUM_ERROR。总尺寸 0x90 与共享 SIG pad 一致。
struct SecuIssueView
{
    char m_pad00[0x4];       // +0x00
    char m_certKey[0x21];    // +0x04
    char m_serial[0xc];      // +0x25
    char m_pad31[0x6];       // +0x31..+0x36
    char m_serverKey[0x27];  // +0x37
    char m_phone[0x5];       // +0x5e
    char m_pad63[0x29];      // +0x63..+0x8b
    ENUM_ERROR m_error;      // +0x8c
};
static_assert(sizeof(SecuIssueView) == 0x90, "SecuIssueView ABI drift");

// SIG_SECURITY_CARD_RETRANSFER 载荷视图（ORIG _sendResult @0x842c7be 写偏移实证）：
// +0x04 strncpy 0x21 / +0x2b strncpy 0x27 / +0x52 strncpy 0xc / +0x88 ENUM_ERROR。
struct SecuRetransferView
{
    char m_pad00[0x4];       // +0x00
    char m_certKey[0x21];    // +0x04
    char m_pad25[0x6];       // +0x25..+0x2a
    char m_serverKey[0x27];  // +0x2b
    char m_serial[0xc];      // +0x52
    char m_pad5e[0x2a];      // +0x5e..+0x87
    ENUM_ERROR m_error;      // +0x88
};
static_assert(sizeof(SecuRetransferView) == 0x8c, "SecuRetransferView ABI drift");

// SIG_QUERY_CHARAC_INFO 载荷视图（ORIG _sendResult @0x8426e04 写偏移实证）：
// +0x00 name strncpy 0x1d / +0x20 errCode / +0x24 characNo / +0x28 job(byte) /
// +0x2a lev(short) / +0x2c growType(byte) / +0x2d sex(byte) / +0x2e 尾 byte。
struct QueryCharacInfoView
{
    char m_name[0x1d];       // +0x00
    char m_pad1d[0x3];       // +0x1d..+0x1f
    int m_errCode;           // +0x20
    int m_characNo;          // +0x24
    unsigned char m_job;     // +0x28
    char m_pad29;            // +0x29
    short m_lev;             // +0x2a
    unsigned char m_growType;// +0x2c
    unsigned char m_sex;     // +0x2d
    unsigned char m_field2e; // +0x2e
    char m_pad2f;            // +0x2f
};
static_assert(sizeof(QueryCharacInfoView) == 0x30, "QueryCharacInfoView ABI drift");

}  // namespace

// ============================================================================
// 1. DB_GetUserRegdate::SendResult —— ORIG 0x0844309c（SendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0xa4c1) → CStreamGuard(stream,true) →
//    <<userIdx → <<slot → put_binary(sig,0xc) → put(DISPATCH_Q) → 返 1。
// ============================================================================
class DB_GetUserRegdate : public DBDispatcher
{
public:
    DB_GetUserRegdate() {}
    bool SendResult(int userIdx, int slot, SIG_GET_USER_REGDATE* sig);
};

#include "OrigDBInter19/SendResult.inc"
                                   SIG_GET_USER_REGDATE* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4c1), true);

    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard->put_binary(sig, 0xc);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// 2. DB_SecurityCardIssue::_sendResult —— ORIG 0x0842b410（_sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0x6dec) → CStreamGuard(stream,true) →
//    <<0xfd → <<param_1 → GetInBuffer<SIG_SECURITY_CARD_ISSUE> → +0x8c=error；
//    param_3→+0x04(0x21)、param_4→+0x37(0x27)、param_5→+0x5e(5)、
//    param_2→+0x25(0xc)，均非空才 strncpy → put(DISPATCH_Q)。void。
// ============================================================================
class DB_SecurityCardIssue : public DBDispatcher
{
public:
    DB_SecurityCardIssue() {}
    void _sendResult(int param_1, char const* param_2, char const* param_3,
                     char const* param_4, char const* param_5, ENUM_ERROR error);
};

#include "OrigDBInter19/_sendResult.inc"
                                       char const* param_3, char const* param_4,
                                       char const* param_5, ENUM_ERROR error)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6dec), true);

    guard.operator*()->operator<<(0xfd);
    guard.operator*()->operator<<(param_1);

    SecuIssueView* view = reinterpret_cast<SecuIssueView*>(
        guard->GetInBuffer<SIG_SECURITY_CARD_ISSUE>());
    view->m_error = error;
    if (param_3 != 0)
    {
        strncpy(view->m_certKey, param_3, 0x21);
    }
    if (param_4 != 0)
    {
        strncpy(view->m_serverKey, param_4, 0x27);
    }
    if (param_5 != 0)
    {
        strncpy(view->m_phone, param_5, 5);
    }
    if (param_2 != 0)
    {
        strncpy(view->m_serial, param_2, 0xc);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ============================================================================
// 3. DB_ReqQueryCharacInfo::_sendResult —— ORIG 0x08426e04（_sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0x6716) → CStreamGuard(stream,true) →
//    <<0xc4 → <<param_2 → GetInBuffer<SIG_QUERY_CHARAC_INFO> → +0x20=errCode；
//    errCode==0 时 strncpy(name,0x1d)、+0x24=characNo、+0x28=(char)job、
//    +0x2a=(short)lev、+0x2c/+0x2d/+0x2e=(char)growType/sex/尾 → put → 返 1。
// ============================================================================
class DB_ReqQueryCharacInfo : public DBDispatcher
{
public:
    DB_ReqQueryCharacInfo() {}
    int _sendResult(int errCode, int slot, char const* name, int characNo,
                    int job, int lev, int growType, int sex, int field2e);
};

#include "OrigDBInter19/_sendResult_1.inc"
                                       int characNo, int job, int lev,
                                       int growType, int sex, int field2e)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6716), true);

    guard.operator*()->operator<<(0xc4);
    guard.operator*()->operator<<(slot);

    QueryCharacInfoView* view = reinterpret_cast<QueryCharacInfoView*>(
        guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>());
    view->m_errCode = errCode;
    if (errCode == 0)
    {
        strncpy(view->m_name, name, 0x1d);
        view->m_characNo = characNo;
        view->m_job = static_cast<unsigned char>(job);
        view->m_lev = static_cast<short>(lev);
        view->m_growType = static_cast<unsigned char>(growType);
        view->m_sex = static_cast<unsigned char>(sex);
        view->m_field2e = static_cast<unsigned char>(field2e);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// 4. DB_RequestTagCharacInfo::sendResult —— ORIG 0x084478a6（sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0xb23f) → CStreamGuard(stream,true) →
//    <<0x2fe → <<param_1 → GetInBuffer<SIG_REQUEST_TAG_CHARAC_INFO> →
//    memcpy(sig,0x414c) → put(DISPATCH_Q)。void。
// ============================================================================
class DB_RequestTagCharacInfo : public DBDispatcher
{
public:
    DB_RequestTagCharacInfo() {}
    void sendResult(int param_1, SIG_REQUEST_TAG_CHARAC_INFO* sig);
};

#include "OrigDBInter19/sendResult.inc"
                                         SIG_REQUEST_TAG_CHARAC_INFO* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb23f), true);

    guard.operator*()->operator<<(0x2fe);
    guard.operator*()->operator<<(param_1);

    SIG_REQUEST_TAG_CHARAC_INFO* out =
        guard->GetInBuffer<SIG_REQUEST_TAG_CHARAC_INFO>();
    memcpy(out, sig, 0x414c);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ============================================================================
// 5. DB_SecurityCardRetransfer::_sendResult —— ORIG 0x0842c7be（_sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0x6fd2) → CStreamGuard(stream,true) →
//    <<0x138 → <<param_1 → GetInBuffer<SIG_SECURITY_CARD_RETRANSFER> →
//    +0x88=error；param_3→+0x04(0x21)、param_4→+0x2b(0x27)、
//    param_2→+0x52(0xc)，均非空才 strncpy → put(DISPATCH_Q)。void。
// ============================================================================
class DB_SecurityCardRetransfer : public DBDispatcher
{
public:
    DB_SecurityCardRetransfer() {}
    void _sendResult(int param_1, char* param_2, char const* param_3,
                     char const* param_4, ENUM_ERROR error);
};

#include "OrigDBInter19/_sendResult_2.inc"
                                            char const* param_3,
                                            char const* param_4, ENUM_ERROR error)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6fd2), true);

    guard.operator*()->operator<<(0x138);
    guard.operator*()->operator<<(param_1);

    SecuRetransferView* view = reinterpret_cast<SecuRetransferView*>(
        guard->GetInBuffer<SIG_SECURITY_CARD_RETRANSFER>());
    view->m_error = error;
    if (param_3 != 0)
    {
        strncpy(view->m_certKey, param_3, 0x21);
    }
    if (param_4 != 0)
    {
        strncpy(view->m_serverKey, param_4, 0x27);
    }
    if (param_2 != 0)
    {
        strncpy(view->m_serial, param_2, 0xc);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}
