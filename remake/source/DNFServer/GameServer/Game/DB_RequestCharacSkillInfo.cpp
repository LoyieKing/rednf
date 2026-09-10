// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_RequestCharacSkillInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "DB_RequestCharacSkillInfo.h"

#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr / s_db_mgr
#include "StreamPool.h"     // StreamPool::Acquire
#include "Stream.h"         // Stream::GetOutBuffer
#include "CStreamGuard.h"   // CStreamGuard(Stream*,bool)/operator<</operator->/GetInBuffer
#include "MsgQueueMgr.h"    // MsgQueueMgr::put + QUEUE_IDX
#include "DBMgr.h"          // DBMgr::GetDBHandle（完整类型）
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP
#include "LogManager.h"
#include "SigTypes_D.h"     // SIG_REQUEST_CHARAC_SKILL_INFO

#include <string.h>         // memcpy

// ---- 本 TU 转接声明（UNLINKED_CALLEE，源码暂无 owner；保持 ORIG 全局
// mangled 名以便链接器命中真实 T 符号）----
// ORIG T _Z19get_skill_slot_namec @0x083fbd93（report 已读：-1/0/其它→"skill_slot"，
//   1→"skill_slot_2nd"）。
const char* get_skill_slot_name(char slot)
{
    if (slot == (char)-1 || slot == 0)
        return "skill_slot";
    if (slot == 1)
        return "skill_slot_2nd";
    return "skill_slot";
}
// ORIG T _Z24get_compressed_blob_dataP5MySQLiPcii @0x083fbc79（定义在 DB_Login.cpp）。
#include "DB_RequestCharacSkillInfo/get_compressed_blob_data.inc"

// ORIG 0x08447162：seLect skill_tree_index from charac_info（DB_HANDLE_2）；
// exec/fetch 失败或 count==0 → logFormat 后返回 0；get_byte(0) 读索引字节。
#include "DB_RequestCharacSkillInfo/_GetSkillTreeIndex.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect skill_tree_index from charac_info where charac_no=%u", characNo);
    bool ok = db->exec(true);
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb19a,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex exec() failed, charac_no(%d)", characNo);
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1a3,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex fetch() failed, charac_no(%d)", characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1aa,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex no result, charac_no(%d)", characNo);
        return 0;
    }
    int i = 0;
    if (!db->get_byte(i++, *outIndex))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1b2,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex get_byte() failed, charac_no(%d)", characNo);
        return 0;
    }
    return 1;
}

// ORIG 0x08447312：seLect %s from skill（DB_HANDLE_3，列名=get_skill_slot_name(slot)）；
// exec/fetch 失败或 count==0 → logFormat 后返回 0；get_compressed_blob_data(0, 0x198, 2)
// 解压已学技能表。
// [b28d-19] N：P75 —— 实参装载时刻（ORIG 在 i++ 前先载 len→%ecx，4.4 末位装载；cast/逗号/具名三形同产）；ORIG 0x08447312。
#include "DB_RequestCharacSkillInfo/_GetSkillInfo.inc"
                                              _Mastered_skill* skills)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    db->set_query("seLect %s from skill where charac_no=%d", get_skill_slot_name(slot), characNo);
    bool ok = db->exec(true);
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1c5,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo exec() failed, charac_no(%d)", characNo);
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1ce,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo fetch() failed, charac_no(%d)", characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1d5,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo no result, charac_no(%d)", characNo);
        return 0;
    }
    int i = 0;
    int len = 0x198;
    // b28 判词（P75 在册 0x08447312）：ORIG 4.1 在 i++ 前先载 len(-0xc)→%ecx 跨
    // 副作用保活；4.4.7 恒在最后装载位实装——实参装载时刻跨版本调度差，
    // cast/逗号/具名三形同产。残差 1 insn（装载位置）封顶。
    if (!get_compressed_blob_data(db, i++, (char*)skills, len, 2))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1e0,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo get_compressed_blob_data failed, charac_no(%d)", characNo);
        return 0;
    }
    return 1;
}

// ORIG 0x084474f2：从 stream 的 out buffer 取 SIG，先取技能树索引，再取技能表，
// 成功才 sendResult(slot, sig)；任一失败返回 0。
#include "DB_RequestCharacSkillInfo/dispatch.inc"
{
    SIG_REQUEST_CHARAC_SKILL_INFO* sig = stream->GetOutBuffer<SIG_REQUEST_CHARAC_SKILL_INFO>();
    bool ok = _GetSkillTreeIndex(*(unsigned int*)&sig->m_pad[0], &sig->m_pad[0x19c]);
    if (!ok)
    {
        return 0;
    }
    ok = _GetSkillInfo(*(unsigned int*)&sig->m_pad[0], *(char*)&sig->m_pad[0x19c],
                       (_Mastered_skill*)((char*)sig + 4));
    if (!ok)
    {
        return 0;
    }
    sendResult(slot, sig);
    return 1;
}

// ORIG 0x084475a0：Acquire 流 → CStreamGuard → <<0x2fd/<<userIdx →
// GetInBuffer<SIG> memcpy 0x1a0 → MsgQueueMgr::put(DISPATCH_Q)。
#include "DB_RequestCharacSkillInfo/sendResult.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb202), true);
    guard.operator*()->operator<<(0x2fd);
    guard.operator*()->operator<<(userIdx);
    SIG_REQUEST_CHARAC_SKILL_INFO* out = guard.operator->()->GetInBuffer<SIG_REQUEST_CHARAC_SKILL_INFO>();
    memcpy(out, sig, 0x1a0);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}
