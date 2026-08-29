// DB 同步 stub 类实现（batch-5 迁移）。
// 6 个 makeRequest 依据 class_func_reports 对应 md + 同族 DB packet 模式实现：
// 包构建约定一致——StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard
// （写 src/字段/header opcode）→ MsgQueueMgr::put(MsgQueueMgr::DB_Q(==2), guard)。
// 各函数 ORIG 地址见注释；字段顺序/长度直接来自反编译 C 与汇编。

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "DB_AccountCargoSync.h"
#include "MySQL.h"
#include "CInventory.h"       // NumberToString(unsigned int, int)
#include "CEnvironment.h"     // DB_HANDLE_2 / E_SERVER_GROUP_NONE
#include <cstring>
#include "GameBasicsScripts.h"   // DB_UpdateAuraAvatarOption
#include "DBMgr.h"           // DBMgr::GetDBHandle
// ============================================================================
// DB_UpgradeAccountCargo::dispatch —— ORIG T 0x0843af76。
// StreamPool::Acquire("DBThread.cpp",0x8bfb) → CStreamGuard → <<userIdx <<slot →
// GetInBuffer<SIG_UPGRADE_ACCOUNT_CARGO_RESULT>（memset 0x8）→ >>u32 a >>u32 b →
// GetDBHandle(DB_HANDLE_2,0) → UPDATE account_cargo；exec 失败返 0；
// affected==0 → flag=0x15 发包(Q1)；否则 flag=0、capacity=b 发包(Q1)。
// ============================================================================
bool DB_UpgradeAccountCargo::dispatch(int userIdx, int slot, Stream* stream)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8bfb);
    CStreamGuard guard(s, true);
    (*guard)->operator<<(userIdx);
    (*guard)->operator<<(slot);
    SIG_UPGRADE_ACCOUNT_CARGO_RESULT* result =
        guard->GetInBuffer<SIG_UPGRADE_ACCOUNT_CARGO_RESULT>();
    memset(result, 0, sizeof(SIG_UPGRADE_ACCOUNT_CARGO_RESULT));
    unsigned int a = 0;
    unsigned int b = 0;
    *stream >> a;
    *stream >> b;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate account_cargo set capacity = %d where m_id = %s",
                  b, NumberToString(a, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        result->m_flag = 0x15;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    result->m_flag = 0;
    result->m_capacity = b;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_CreateAccountCargo::makeRequest ORIG @ 0x0843ae86
//   opcode 0x1ee | param_1(int uid) | param_2(uint accId) | param_3(uint level)
// ============================================================================
void DB_CreateAccountCargo::makeRequest(int param_1, unsigned int param_2,
                                        unsigned int param_3)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8bef);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x1ee);                 // SIG_CREATE_ACCOUNT_CARGO
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<(param_3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_DeleteAccountCargo::makeRequest ORIG @ 0x0843b2e0
//   opcode 0x1f0 | param_1(int uid) | param_2(uint accId)
// ============================================================================
void DB_DeleteAccountCargo::makeRequest(int param_1, unsigned int param_2)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c38);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x1f0);                 // SIG_DELETE_ACCOUNT_CARGO
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpgradeAccountCargo::makeRequest ORIG @ 0x0843b15e
//   opcode 0x1ef | param_1(int uid) | param_2(uint accId) | param_3(uint level)
// ============================================================================
void DB_UpgradeAccountCargo::makeRequest(int param_1, unsigned int param_2,
                                         unsigned int param_3)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c1d);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x1ef);                 // SIG_UPGRADE_ACCOUNT_CARGO
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<(param_3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_GuildExpBookDelete::makeRequest ORIG @ 0x0843c408
//   opcode 0x209 | -1 | param_1(uint) | param_2(int) | param_3(ENUM_SERVER_GROUP)
//   注意 ORIG 在三个参数前固定写 -1（int）。（分段推断：-1 为 ORIG 常量。）
// ============================================================================
void DB_GuildExpBookDelete::makeRequest(unsigned int param_1, int param_2,
                                        ENUM_SERVER_GROUP param_3)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x902a);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x209);                 // SIG_GUILD_EXP_BOOK_DELETE
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<((int)param_3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_InsertRandomOptionItemInform::makeRequest ORIG @ 0x08442d70
//   opcode 0x299 | param_1(int uid) | param_2(uint accId) | param_3(uchar 零扩展 uint)
// ============================================================================
void DB_InsertRandomOptionItemInform::makeRequest(int param_1,
                                                  unsigned int param_2,
                                                  unsigned char param_3)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa488);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x299);                 // SIG_INSERT_RANDOM_OPTION_ITEM_INFORM
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<(param_3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_PassPadUpdateFailCnt::makeRequest ORIG @ 0x08423eb4
//   opcode 0xa4 | -1，随后在 GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>
//   缓冲写入：+0x00 param_1(accId)、+0x04 param_2(failCnt)、
//   +0x08 strncpy(param_3, 0x10 webAddr)。ORIG 由 dispatch @0x08423bc4 如此消费
//   （+0 m_id、+4 fail_cnt、+8 ip_info）。
// ============================================================================
void DB_PassPadUpdateFailCnt::makeRequest(unsigned int param_1,
                                          unsigned int param_2, char* param_3)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5f31);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0xa4);                  // SIG_SECURITY_CARD_UPDATE_FAIL_CNT
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    char* p = (char*)sig;
    *(unsigned int*)(p + 0x0) = param_1;   // accId (m_id)
    *(unsigned int*)(p + 0x4) = param_2;   // failCnt
    strncpy(p + 0x8, param_3, 0x10);       // web address (ip_info)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_PassPadUpdateCancelCnt::makeRequest ORIG @ 0x0842403a
//   opcode 0xa5 | -1，随后在 GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>
//   缓冲写入：+0x00 param_1(accId)、+0x04 param_2(cancelCnt)、
//   +0x08 strncpy(param_3, 0x10 webAddr)。
// ============================================================================
void DB_PassPadUpdateCancelCnt::makeRequest(unsigned int param_1,
                                            unsigned int param_2,
                                            char* param_3)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5f5e);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0xa5);                  // SIG_SECURITY_CARD_UPDATE_CANCEL_CNT
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    char* p = (char*)sig;
    *(unsigned int*)(p + 0x0) = param_1;   // accId
    *(unsigned int*)(p + 0x4) = param_2;   // cancelCnt
    strncpy(p + 0x8, param_3, 0x10);       // web address
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_SecurityCardUpdateCancelCnt::makeRequest ORIG @ 0x0842c112
//   opcode 0x102 | -1，缓冲 +0x00 param_1(accId)、+0x04 param_2(cancelCnt)。
// ============================================================================
void DB_SecurityCardUpdateCancelCnt::makeRequest(unsigned int param_1,
                                                 unsigned int param_2)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6f25);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x102);                 // SIG_SECURITY_CARD_UPDATE_CANCEL_CNT
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    char* p = (char*)sig;
    *(unsigned int*)(p + 0x0) = param_1;   // accId
    *(unsigned int*)(p + 0x4) = param_2;   // cancelCnt
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// DB_UpdateAuraAvatarOption::makeRequest 权威实现见 W8LT_DBTask.cpp（FixCall_D 统一）。

namespace APSystem
{
void DB_UpdateActionPoint::makeRequest(int, const _SIG_LOAD_ACTION_POINT&, bool)
{
}  // TODO(G2)
}