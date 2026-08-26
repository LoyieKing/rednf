// df_game_r 还原 —— LinkMisc4 批次 DB 请求 TU（4 个 makeRequest）。
// 逐函数对照 docs/class_func_reports/{DB_EventGrowthWeaponReset,
// DB_EventUpdateGrowthWeaponTimepiece,DB_ReqChangeCharacName,
// DB_TestTradeRestrict}/makeRequest.md。语义：向 DB 队列（QUEUE_IDX = 2 = DB_Q）
// 发一条 CStreamGuard 请求；表头值、字段写入顺序、Acquire 行号常量均取自报告。
// 声明唯一来源为各自同名头文件。
#include <cstring>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "SigTypes.h"
#include "DB_EventGrowthWeaponReset.h"
#include "DB_EventUpdateGrowthWeaponTimepiece.h"
#include "DB_ReqChangeCharacName.h"
#include "DB_TestTradeRestrict.h"

// ============================================================================
// DB_EventGrowthWeaponReset::makeRequest @ 0x08449980
// mangled _ZN25DB_EventGrowthWeaponReset11makeRequestEj
// Acquire("DBThread.cpp", 0xb95e)；<<0x316(int) → <<-1 → <<accId(u32 0x080e2292)
// ============================================================================
void DB_EventGrowthWeaponReset::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb95e), true);
    guard.operator*()->operator<<(0x316);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_EventUpdateGrowthWeaponTimepiece::makeRequest @ 0x084497ea
// mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece11makeRequestEjsi
// Acquire("DBThread.cpp", 0xb941)；<<0x315 → <<-1 → <<accId(u32) →
// <<timepiece(short 0x0822ad66) → <<value(int 0x080c8c56)
// ============================================================================
void DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
    unsigned int accId, short timepiece, int value)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb941), true);
    guard.operator*()->operator<<(0x315);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(timepiece);   // operator<<(short) 0x0822ad66
    guard.operator*()->operator<<(value);       // operator<<(int) 0x080c8c56
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_ReqChangeCharacName::makeRequest @ 0x08426590
// mangled _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
// Acquire("DBThread.cpp", 0x64de)；<<0xc3(int) → <<accId(int) →
// GetInBuffer<SIG_CHANGE_CHARAC_NAME> 直填 +0x00/+0x04(strncpy 0x1d)/+0x24/
// +0x2c（渠道组低字节，ORIG mov 0x378(%eax),%eax; mov %dl,0x2c）。
// SIG_CHANGE_CHARAC_NAME 当前权威布局为 char m_pad[0x30]（SigTypes.h），
// 按报告偏移写入。
// ============================================================================
void DB_ReqChangeCharacName::makeRequest(int accId, int characIdx,
                                         char const* name, int flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x64de), true);
    guard.operator*()->operator<<(0xc3);
    guard.operator*()->operator<<(accId);       // operator<<(int) 0x080c8c56

    // ORIG 请求体具名视图（总 0x30，与 SIG_CHANGE_CHARAC_NAME 布局一致）：
    // +0x00 characIdx / +0x04 name[0x20] / +0x24 flag / +0x2c 渠道组字节。
    // 渠道组 = CEnvironment+0x378（m_serverEnvir.m_serverGroup）低字节。
    struct ChangeCharacNameBody
    {
        int characIdx;      // +0x00
        char name[0x20];    // +0x04（strncpy 写 0x1d）
        int flag;           // +0x24
        char gap[4];        // +0x28（ORIG 保留区）
        char group;         // +0x2c
    };
    ChangeCharacNameBody* body = reinterpret_cast<ChangeCharacNameBody*>(
        guard.operator->()->GetInBuffer<SIG_CHANGE_CHARAC_NAME>());
    body->characIdx = characIdx;
    strncpy(body->name, name, 0x1d);
    body->flag = flag;
    body->group =
        static_cast<char>(G_CEnvironment()->m_serverEnvir.m_serverGroup);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_TestTradeRestrict::makeRequest @ 0x0844a132
// mangled _ZN20DB_TestTradeRestrict11makeRequestEj
// Acquire("DBThread.cpp", 0xba73)；<<0x31c(int) → <<-1 → <<accId(u32)
// ============================================================================
void DB_TestTradeRestrict::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xba73), true);
    guard.operator*()->operator<<(0x31c);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
