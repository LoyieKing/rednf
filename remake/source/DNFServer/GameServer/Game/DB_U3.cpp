// U3 杂项 DB 同步请求 TU（U3ShopDb 批次，2026-08-23）。
// 两个 DB packet makeRequest，逐字段对照 docs/class_func_reports 反编译 C：
//   DB_AccountHack_CheckMyAccount::makeRequest  0x0843a13e
//   DB_InsertCeraItemHistory::makeRequest       0x0844511a
// 包构建约定：StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard →
// 写 opcode/字段 → MsgQueueMgr::put(DB_Q, guard)。
// 类声明与 CMailBoxHelper.cpp 中的前置声明逐字一致（ORIG 返回 void，此处
// 沿用调用方声明的 int 以保持类定义一致；mangled 符号不含返回类型）。

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "StreamDeps.h"       // G_CEnvironment（UnlimitSupportLog serverGroup 字段）
#include <cstring>

class DB_AccountHack_CheckMyAccount
{
public:
    static int makeRequest(unsigned int uid, unsigned int accId,
                           unsigned int a, unsigned int money,
                           char* senderName, int nameLen);
};

class DB_InsertCeraItemHistory
{
public:
    static int makeRequest(int itemIdx, unsigned int senderCharacNo,
                           unsigned int receiverCharacNo, char flag);
};

// ---- DB_AccountHack_CheckMyAccount::makeRequest ORIG @ 0x0843a13e ----
// stream: opcode 0x18b | param_1(uint)
// in-buffer(SIG_CHECK_MY_ACCOUNT_IN_DB): +0x00=param_3, +0x04=param_2,
//   +0x08 memset 0x1d 后 strncpy(param_5, param_6), +0x25=param_4
int DB_AccountHack_CheckMyAccount::makeRequest(unsigned int uid,
                                               unsigned int accId,
                                               unsigned int a,
                                               unsigned int money,
                                               char* senderName, int nameLen)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8846);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x18b);  // SIG_CHECK_MY_ACCOUNT
    guard.operator*()->operator<<((int)uid);

    SIG_CHECK_MY_ACCOUNT_IN_DB* sig =
        guard.GetInBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>();
    char* base = reinterpret_cast<char*>(sig);
    *reinterpret_cast<unsigned int*>(base + 0x00) = a;
    *reinterpret_cast<unsigned int*>(base + 0x04) = accId;
    *reinterpret_cast<unsigned int*>(base + 0x25) = money;
    std::memset(base + 0x08, 0, 0x1d);
    std::strncpy(base + 0x08, senderName, nameLen);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

// ---- DB_InsertCeraItemHistory::makeRequest ORIG @ 0x0844511a ----
// stream: opcode 0x2b1 | -1 | itemIdx(int) | sender(uint) | receiver(uint)
//         | flag(char 符号扩展)
int DB_InsertCeraItemHistory::makeRequest(int itemIdx,
                                          unsigned int senderCharacNo,
                                          unsigned int receiverCharacNo,
                                          char flag)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xac1b);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x2b1);          // SIG_INSERT_CERA_ITEM_HISTORY
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(itemIdx);
    guard.operator*()->operator<<((int)senderCharacNo);
    guard.operator*()->operator<<((int)receiverCharacNo);
    guard.operator*()->operator<<(flag);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

// ============================================================================
// Prod_DB_Misc 批次：DB_GetDeathTowerBestRecord / DB_GetUserRegdate
// （DisPatcherCore.cpp SelectCharac 调用；类声明与该 TU 前置声明逐字一致，
//   参数类型按 ORIG mangled Eij = (int, unsigned int)）
// ============================================================================

class DB_GetDeathTowerBestRecord
{
public:
    static void makeRequest(int uid, unsigned int characNo);
};

class DB_GetUserRegdate
{
public:
    static void makeRequest(int uid, unsigned int accId);
};

// ---- DB_GetDeathTowerBestRecord::makeRequest ORIG @ 0x0842a048 ----
// stream: opcode 0xe2 | uid(int) | characNo(uint)，投 DB_Q。
void DB_GetDeathTowerBestRecord::makeRequest(int uid, unsigned int characNo)
{
    // ORIG 直接把 Acquire 返回值传入守卫构造（无中间局部，栈帧一致）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6be0), true);
    guard.operator*()->operator<<(0xe2);           // SIG_GET_DEATH_TOWER_BEST_RECORD
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(characNo);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- DB_GetUserRegdate::makeRequest ORIG @ 0x8442fae ----
// stream: opcode 0x2a1 | uid(int)；GetInBuffer<SIG_GET_USER_REGDATE>：
//   +0x00=accId（dispatch 作 SQL where m_id）、+0x04/+0x08 清零（dispatch 回填）。
void DB_GetUserRegdate::makeRequest(int uid, unsigned int accId)
{
    // ORIG 直接把 Acquire 返回值传入守卫构造（无中间局部，栈帧一致）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4b2), true);
    guard.operator*()->operator<<(0x2a1);          // SIG_GET_USER_REGDATE
    guard.operator*()->operator<<(uid);

    // ORIG：call operator->(080c8c4e) 后调 CStreamGuard::GetInBuffer<W 0x08453e72>；
    // 清零顺序按汇编 +0x00 → +0x08 → +0x04。
    SIG_GET_USER_REGDATE* sig =
        guard.operator->()->GetInBuffer<SIG_GET_USER_REGDATE>();
    sig->m_id = accId;
    sig->m_regDate = 0;
    sig->m_regDay = 0;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DBHistoryDeps 批次（2026-08-24）：DB_InsertArchieveEventLog /
// DB_InsertUnlimitSupportLog / DB_ReqSaveHackUserPunish::makeRequest。
// 类声明与调用方 TU 局部前置声明逐字一致（CUserStatA.cpp / DisPatcherCore.cpp），
// 返回 void 按 ORIG 反编译；符号即 ORIG T：
//   _ZN25DB_InsertArchieveEventLog11makeRequestEjjN24SIG_INSERT_ACHIEVE_EVENT17ENUM_ACHIEVE_TYPEE
//   _ZN26DB_InsertUnlimitSupportLog11makeRequestEjjPKc
//   _ZN24DB_ReqSaveHackUserPunish11makeRequestEjjPKc
// ============================================================================

class DB_InsertArchieveEventLog
{
public:
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type);
};

class DB_InsertUnlimitSupportLog
{
public:
    static void makeRequest(unsigned int uid, unsigned int characNo,
                            const char* name);
};

class DB_ReqSaveHackUserPunish
{
public:
    static void makeRequest(unsigned int uid, unsigned int type, const char* ip);
};

// ---- DB_InsertArchieveEventLog::makeRequest ORIG @ 0x0842e856 ----
// stream: opcode 0x117 | -1 | GetInBuffer<SIG_INSERT_ACHIEVE_EVENT>：
//   +0x00=accId、+0x04=characNo、+0x08=type。投 LOGDB_Q（ORIG put 第 2 实参 4）。
// SIG 结构在 SigTypes.h 为 0x100 占位缓冲，按报告偏移以函数内命名视图字段化
// （成员访问折叠出与 ORIG 相同的 mov %edx,0xN(%eax) 存储形态）。
void DB_InsertArchieveEventLog::makeRequest(
    unsigned int accId, unsigned int characNo,
    SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7349), true);
    guard.operator*()->operator<<(0x117);          // SIG_INSERT_ACHIEVE_EVENT
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_characNo;                   // +0x04
        SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE m_type;  // +0x08（原枚举存储）
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_INSERT_ACHIEVE_EVENT>());
    v->m_accId = accId;
    v->m_characNo = characNo;
    v->m_type = type;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ---- DB_InsertUnlimitSupportLog::makeRequest ORIG @ 0x0842ebd0 ----
// ORIG 首指令 cmpl $0x946501,param_1; jbe → uid <= 0x946501 直接返回。
// GetInBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>：+0x00=uid、+0x04=serverGroup
// （G_CEnvironment()+0x378 == m_serverEnvir.m_serverGroup，具名链见 CEnvironment.h）、
// +0x08=characNo、+0x0c strncpy(name,0x1d)（ORIG 无 memset）。投 DB_Q（实参 2）。
void DB_InsertUnlimitSupportLog::makeRequest(unsigned int uid, unsigned int characNo,
                                             const char* name)
{
    if (uid <= 0x946501)
        return;

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7392), true);
    guard.operator*()->operator<<(0x122);          // SIG_INSERT_UNLIMIT_SUPPORT_LOG
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_serverGroup;                // +0x04
        unsigned int m_characNo;                   // +0x08
        char m_name[0x1d];                         // +0x0c
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>());
    v->m_accId = uid;
    v->m_characNo = characNo;
    // 报告语句序：先写 +8，再调 G_CEnvironment 回填 +4（serverGroup）。
    v->m_serverGroup =
        static_cast<unsigned int>(G_CEnvironment()->m_serverEnvir.m_serverGroup);
    std::strncpy(v->m_name, name, 0x1d);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- DB_ReqSaveHackUserPunish::makeRequest ORIG @ 0x08425494 ----
// GetInBuffer<SIG_SAVE_HACK_USER_PUNISH>：+0x00=uid、+0x04=type、+0x08 ip：
//   NULL → memcpy "000.000.000.000"（0x10 含 NUL）；否则 strncpy(ip,0x10)。
//   两分支 ORIG 形态不同（add 直算 vs lea 经 edx），按原表达式转写。投 DB_Q。
void DB_ReqSaveHackUserPunish::makeRequest(unsigned int uid, unsigned int type,
                                           const char* ip)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6304), true);
    guard.operator*()->operator<<(0xb5);           // SIG_SAVE_HACK_USER_PUNISH
    guard.operator*()->operator<<(-1);

    struct View
    {
        unsigned int m_accId;                      // +0x00
        unsigned int m_type;                       // +0x04
        char m_ip[0x10];                           // +0x08
    };
    View* v =
        reinterpret_cast<View*>(guard.operator->()->GetInBuffer<SIG_SAVE_HACK_USER_PUNISH>());
    v->m_accId = uid;
    v->m_type = type;
    if (ip == 0)
        std::memcpy(reinterpret_cast<char*>(v) + 8, "000.000.000.000", 0x10);
    else
        std::strncpy(v->m_ip, ip, 0x10);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
