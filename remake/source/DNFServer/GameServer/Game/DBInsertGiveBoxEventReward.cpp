// df_game_r 还原 —— DBInsertGiveBoxEventReward::makeRequest（礼盒事件奖励请求）。
// ORIG 0x0815d2c8（mangled _ZN26DBInsertGiveBoxEventReward11makeRequestEP5CUser，
// 0x126 字节）：Acquire("DBThread.cpp",0x369) → CStreamGuard(stream,true) →
// Taiwan::internal_stream(guard, 0x10, user->GetUID()) → GetInBuffer<MSG>() 写
// m_accId/m_characNo → DB_Q put；缓冲为空走 cMyTrace。
#include "DBInsertGiveBoxEventReward.h"

#include "CUser.h"
#include "MoreTypes.h"       // DBInsertGiveBoxEventReward::MSG
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "TaiwanInternal.h"
#include "GameTypes.h"       // cMyTrace

void DBInsertGiveBoxEventReward::makeRequest(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x369), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x10,
                            user->GetUID());
    MSG* msg = guard->GetInBuffer<MSG>();
    if (msg == 0)
    {
        cMyTrace trace("DBThread.cpp", 0x36e, 0);
        trace("[Taiwan, Event] msg is null.");
    }
    else
    {
        msg->m_accId = user->get_acc_id();
        msg->m_characNo = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}
