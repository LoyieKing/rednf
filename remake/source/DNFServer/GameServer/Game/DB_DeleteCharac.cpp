// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_DeleteCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "DB_DeleteCharac.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"         // logFormat
#include "CEnvironment.h"       // G_CEnvironment / Server_Envir::m_serverGroup
#include "DB_LoadInventory.h"   // DB_LoadInventory::GetItemLock（static）
#include "CTitleBook.h"         // stItemLockInfo（物品锁 DB 值类型）

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

// ============================================================================
// DB_DeleteCharac::DB_DeleteCharac @0x08402bce（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1 0x08402bea / D0 0x08402c1a deleting）由类定义产出。
// ============================================================================
#include "DB_DeleteCharac/DB_DeleteCharac.inc"
{
}

// ============================================================================
// ~DB_DeleteCharac @0x08402bea（D1）/ 0x08402c1a（D0 deleting）
// ============================================================================
#include "DB_DeleteCharac/dtor_DB_DeleteCharac.inc"
{
}

// ============================================================================
// DB_DeleteCharac::dispatch @0x08402c38
// 基类 dispatch 通过 → GetOutBuffer<SIG_DELETE_CHARAC>（空 → 0）→
// +0x8 清零 → getCurSec → stItemLockInfo[100] 自动构造 → GetItemLock
// 成功则扫描 100 项：state==1 或 (state==2 且 unlockSec+0x3f480>curSec)
// → +0x8=3 + SendResult 返 1。DeleteGuildInfo 失败 → cMyTrace 0x12a7 +
// SendResult；成功 → +0x10=0，CheckEventCharacter40 为真则
// DeleteEventCharacFlag + +0x10=1；DeleteCharac 失败 +0x8=1；
// DeleteCharacOption；SendResult 失败 → 0；成功 cMyTrace 0x12ce 返 1。
// ============================================================================
#include "DB_DeleteCharac/dispatch.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return 0;
    }

    bool ret;
    SIG_DELETE_CHARAC* sig = stream->GetOutBuffer<SIG_DELETE_CHARAC>();
    if (sig == 0)
    {
        return 0;
    }
    sig->m_errNo = 0;

    int curSec = GlobalData::s_systemTime_.getCurSec();

    stItemLockInfo lock[100];
    if (DB_LoadInventory::GetItemLock(sig->m_characNo, lock) == 0)
    {
        for (int i = 0; i <= 99; ++i)
        {
            if (lock[i].m_state == 1 ||
                (lock[i].m_state == 2 &&
                 lock[i].m_unlockSec + 0x3f480 > curSec))
            {
                sig->m_errNo = 3;
                if (!SendResult(userIdx, slot, sig))
                {
                    return 0;
                }
                return 1;
            }
        }
    }

    if (!DeleteGuildInfo(sig))
    {
        cMyTrace("virtual bool DB_DeleteCharac::dispatch(int, int, Stream*)",
                 0x12a7, 0)("[%s][%d] deLete charac(%d),guild_id(%d) FAIL",
                            __PRETTY_FUNCTION__, 0x12a7, sig->m_characNo,
                            sig->m_guildId);
        if (!SendResult(userIdx, slot, sig))
        {
            return 0;
        }
        return 1;
    }

    sig->m_eventFlag = 0;
    if (CheckEventCharacter40(sig))
    {
        DeleteEventCharacFlag(sig);
        sig->m_eventFlag = 1;
    }
    ret = DeleteCharac(sig);
    if (!ret)
    {
        sig->m_errNo = 1;
    }
    DeleteCharacOption(sig);
    ret = SendResult(userIdx, slot, sig);
    if (!ret)
    {
        return 0;
    }
    cMyTrace("virtual bool DB_DeleteCharac::dispatch(int, int, Stream*)",
             0x12ce, 0)("[%s][%d] deLete charac(%d),guild_id(%d)",
                        __PRETTY_FUNCTION__, 0x12ce, sig->m_characNo,
                        sig->m_guildId);
    return 1;
}

// ============================================================================
// DB_DeleteCharac::DeleteGuildInfo @0x08402f4c
// guildId==0 → charac_info 查 guild_id 入 +0xc（失败/空 +0x8=1 返 0；
// 查到仍 0 → logFormat 0x1312 返 1）。guild_info 查 master_no：
// 失败 +0x8=1；characNo==master → +0x8=2 返 0；否则 guild_member 退会
// （member_flag=2/secede），count(*) 更新 guild_info.member_count。
// ============================================================================
#include "DB_DeleteCharac/DeleteGuildInfo.inc"
{
    if (sig->m_guildId == 0)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                      (ENUM_SERVER_GROUP)0);
        db->set_query("seLect guild_id from charac_info where charac_no = %d",
                      sig->m_characNo);
        if (!db->exec(true))
        {
            sig->m_errNo = 1;
            return 0;
        }
        if (!db->fetch() || !db->get_uint(0, sig->m_guildId))
        {
            sig->m_errNo = 1;
            return 0;
        }
        if (sig->m_guildId == 0)
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0x1312, "[Guild Del Error] guild_id=0, charac_no:%d",
                sig->m_characNo);
            return 1;
        }
    }

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)8,
                                                   (ENUM_SERVER_GROUP)0);
    db2->set_query("seLect master_no from guild_info where guild_id = %d",
                   sig->m_guildId);
    if (!db2->exec(true))
    {
        sig->m_errNo = 1;
        return 0;
    }
    unsigned int masterNo = 0;
    if (!db2->fetch() || !db2->get_uint(0, masterNo))
    {
        sig->m_errNo = 1;
        return 0;
    }
    if (sig->m_characNo == masterNo)
    {
        sig->m_errNo = 2;
        return 0;
    }
    db2->set_query("upDate guild_member set member_flag = 2, secede_time = "
                   "now(), secede_type = 2 where guild_id = %d and "
                   "charac_no = %d", sig->m_guildId, sig->m_characNo);
    if (!db2->exec(true))
    {
        sig->m_errNo = 1;
        return 0;
    }
    db2->set_query("seLect count(*) from guild_member where guild_id = %d and "
                   "member_flag = 1", sig->m_guildId);
    if (!db2->exec(true))
    {
        sig->m_errNo = 1;
        return 0;
    }
    int count;
    if (!db2->fetch() || !db2->get_int(0, count))
    {
        sig->m_errNo = 1;
        return 0;
    }
    if (count != 0)
    {
        db2->set_query("upDate guild_info set member_count = %d where "
                       "guild_id = %d", count, sig->m_guildId);
        if (!db2->exec(true))
        {
            sig->m_errNo = 1;
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_DeleteCharac::DeleteEventCharacFlag @0x084032aa
// GetDBHandle(6) → NumberToString(m_accId) → member_login 清
// event_charac_flag。exec 失败返 0。
// ============================================================================
#include "DB_DeleteCharac/DeleteEventCharacFlag.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate member_login set event_charac_flag = 0 where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_DeleteCharac::CheckEventCharacter40 @0x08403324
// charac_info 查 event_charac_level（m_id=accId 串、delete_flag=0）；
// exec/fetch/get_int 失败 → 0；level==0 → 0；否则 1。
// ============================================================================
#include "DB_DeleteCharac/CheckEventCharacter40.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect event_charac_level from charac_info where m_id=%s "
                  "and charac_no=%d and delete_flag=0",
                  NumberToString(sig->m_accId, 0), sig->m_characNo);
    if (!db->exec(true))
    {
        return 0;
    }
    int level = 0;
    if (!db->fetch() || !db->get_int(0, level))
    {
        return 0;
    }
    if (level != 0)
    {
        return 1;
    }
    return 0;
}

// ============================================================================
// DB_DeleteCharac::DeleteCharac @0x0840340c
// charac_info 置 delete_time=now()/delete_flag=1（charac_no=%u and m_id=%s）；
// exec 失败返 0。成功 → GetDBHandle(1) 从 dnf_master_charac 删除
// （m_id/servier_id=Server_Envir.m_serverGroup/charac_no）后返 1。
// ============================================================================
#include "DB_DeleteCharac/DeleteCharac.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate charac_info set delete_time=now(),delete_flag=1 "
                  "where charac_no=%u and m_id=%s", sig->m_characNo,
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                   (ENUM_SERVER_GROUP)0);
    db2->set_query("deLete from dnf_master_charac where m_id='%u' and "
                   "server_id='%u' and charac_no='%u'", sig->m_accId,
                   G_CEnvironment()->m_serverEnvir.m_serverGroup,
                   sig->m_characNo);
    db2->exec(true);
    return 1;
}

// ============================================================================
// DB_DeleteCharac::SendResult @0x084034fe
// StreamPool::Acquire("DBThread.cpp",0x13b0) → guard<<userIdx<<slot →
// put_binary(sig,0x14) → MsgQueueMgr::put(QUEUE_IDX 1)。恒返 1。
// ============================================================================
#include "DB_DeleteCharac/SendResult.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp",
                                                           0x13b0), true);
    **guard << userIdx;
    **guard << slot;
    guard->put_binary(sig, 0x14);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return true;
}

// ============================================================================
// DB_DeleteCharac::DeleteCharacOption @0x084035e0
// charac_option 按 charac_no 删除。exec 失败返 0。
// ============================================================================
#include "DB_DeleteCharac/DeleteCharacOption.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("deLete from charac_option where charac_no = %u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return 0;
    }
    return 1;
}
