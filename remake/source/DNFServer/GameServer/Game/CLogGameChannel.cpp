// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLogGameChannel/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CLogGameChannel.h"
#include "SigTypes.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "TimerQueue.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CEnvironment.h"
#include <string.h>

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit；GetLogTargetDB
// 定义前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN8WongWork15CLogGameChannel14GetLogTargetDBEv`（ORIG 0x8550763）。
#include <iostream>

// df_game_r 还原 —— WongWork::CLogGameChannel（LogGameChannel 定时落库）。
// 参照 docs/class_func_reports/WongWork__CLogGameChannel/ 反汇编逐函数实现。
// 布局见 CLogGameChannel.h（ctor 0x082a71c4：+0/+4/+8 清零，+0xc period=10）。

namespace WongWork
{

// ORIG 0x085503c4
#include "CLogGameChannel/GetLogTargetDB.inc"
{
    return GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                             E_SERVER_GROUP_NONE);
}

// ORIG 0x082a71c4 W
#include "CLogGameChannel/CLogGameChannel.inc"
{
    m_inUser = 0;
    m_outUser = 0;
    m_currentUser = 0;
    m_period = 10;
}

// ORIG 0x085503ea
#include "CLogGameChannel/StartLogTimer.inc"
{
    SetNextLogTimer();
}

// ORIG 0x085503fe
#include "CLogGameChannel/SetNextLogTimer.inc"
{
    // InsertTimer(OBJ_TYPE_2, 0, TIMER_MESSAGE_0x58, period*60, 0, 0)
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x58, m_period * 0x3c, 0, 0);
}

// ORIG 0x085506de
#include "CLogGameChannel/IncInUser.inc"
{
    m_inUser++;
    m_currentUser++;
}

// ORIG 0x08550700
#include "CLogGameChannel/IncOutUser.inc"
{
    m_outUser++;
    m_currentUser--;
}

// ORIG 0x085506c6
#include "CLogGameChannel/ResetInOutUser.inc"
{
    m_inUser = 0;
    m_outUser = 0;
}

// ORIG 0x085503ea
#include "CLogGameChannel/ReqSaveLog.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("LogGameChannel.cpp", 0x20), true);
    guard.operator*()->operator<<(0x79);
    guard.operator*()->operator<<(-1);

    SIG_SAVE_LOGGAMECHANNEL* pSig =
        guard->GetInBuffer<SIG_SAVE_LOGGAMECHANNEL>();
    memset(pSig, 0, 0x3b);

    pSig->m_game = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    pSig->m_chGroup = (short)G_CEnvironment()->m_serverEnvir.m_channelNo;
    pSig->m_no = G_CEnvironment()->m_serverEnvir.m_gcNo;
    memcpy(pSig->m_channel, G_CEnvironment()->m_serverEnvir.m_channelName,
           strlen(G_CEnvironment()->m_serverEnvir.m_channelName));
    memcpy(pSig->m_ip, G_CEnvironment()->m_serverEnvir.m_serverIP,
           strlen(G_CEnvironment()->m_serverEnvir.m_serverIP));
    pSig->m_now = (short)m_currentUser;
    pSig->m_new = (short)m_inUser;
    pSig->m_out = (short)m_outUser;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ORIG 0x08550612
#include "CLogGameChannel/DoSaveLog.inc"
{
    MySQL* db = GetLogTargetDB();
    db->set_query(
        "inSert into log_game_channel(gc_up_time,gc_game,gc_no,gc_channel,"
        "gc_ch_group,gc_ip,gc_now,gc_new,gc_out) values(now(),%d,%d,'%s',%d,"
        "'%s',%d,%d,%d)",
        (int)(char)pSig->m_game,
        pSig->m_no,
        pSig->m_channel,
        (int)pSig->m_chGroup,
        pSig->m_ip,
        (int)pSig->m_now,
        (int)pSig->m_new,
        (int)pSig->m_out);
    db->exec(true);
}

} // namespace WongWork
