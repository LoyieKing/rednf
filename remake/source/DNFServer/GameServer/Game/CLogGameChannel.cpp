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

// df_game_r 还原 —— WongWork::CLogGameChannel（LogGameChannel 定时落库）。
// 参照 docs/class_func_reports/WongWork__CLogGameChannel/ 反汇编逐函数实现。
// 布局见 CLogGameChannel.h（ctor 0x082a71c4：+0/+4/+8 清零，+0xc period=10）。

namespace WongWork
{

// ORIG 0x082a71c4 W
CLogGameChannel::CLogGameChannel()
{
    m_inUser = 0;
    m_outUser = 0;
    m_currentUser = 0;
    m_period = 10;
}

// ORIG 0x085503ea
void CLogGameChannel::StartLogTimer()
{
    SetNextLogTimer();
}

// ORIG 0x085503fe
void CLogGameChannel::SetNextLogTimer()
{
    // InsertTimer(OBJ_TYPE_2, 0, TIMER_MESSAGE_0x58, period*60, 0, 0)
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x58, m_period * 0x3c, 0, 0);
}

// ORIG 0x085506de
void CLogGameChannel::IncInUser()
{
    m_inUser++;
    m_currentUser++;
}

// ORIG 0x08550700
void CLogGameChannel::IncOutUser()
{
    m_outUser++;
    m_currentUser--;
}

// ORIG 0x085506c6
void CLogGameChannel::ResetInOutUser()
{
    m_inUser = 0;
    m_outUser = 0;
}

// ORIG 0x085503c4
MySQL* CLogGameChannel::GetLogTargetDB()
{
    return GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                             E_SERVER_GROUP_NONE);
}

// ORIG 0x08550456
void CLogGameChannel::ReqSaveLog()
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
void CLogGameChannel::DoSaveLog(SIG_SAVE_LOGGAMECHANNEL* pSig)
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
