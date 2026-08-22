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
#include <cstring>
#include "GameBasicsScripts.h"   // DB_UpdateAuraAvatarOption

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
    guard.operator*()->operator<<((unsigned int)param_3);  // 反编译 movzbl 零扩展
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
                                            const char* param_3)
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

// ============================================================================
// DB_UpdateAuraAvatarOption::makeRequest ORIG @ 0x08444f26
//   opcode 0x2af | -1 | param_2(uint characNo) | param_3(int idx) | param_4(int value)
// ============================================================================
void DB_UpdateAuraAvatarOption::makeRequest(int param_1, unsigned int param_2,
                                            int param_3, int param_4)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xabf6);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x2af);                 // SIG_UPDATE_AURA_AVATAR_OPTION
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<(param_3);
    guard.operator*()->operator<<(param_4);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

namespace APSystem
{
void DB_UpdateActionPoint::makeRequest(int, const _SIG_LOAD_ACTION_POINT&, bool)
{
}  // TODO(G2)
}