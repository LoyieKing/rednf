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

// b28c-L20: AE — ORIG 0x0815d2c8 orig=77 our=77 gap=0 ndiff=0。两处修正：
//   ①Acquire 文件名串必须 "localtaiwan/Dispatcher/Taiwan_DbThread.cpp"（原 "DBThread.cpp" 为真 bug）；
//   ②cMyTrace 用 __PRETTY_FUNCTION__（P139）+ 全表达式临时链（P221/P227）：具名对象
//   使 guard(-0x24)/trace(-0x1c) 槽号互换（ndiff 16），临时链后槽深与 ORIG 一致。
void DBInsertGiveBoxEventReward::makeRequest(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x369), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x10,
                            user->GetUID());
    MSG* msg = guard->GetInBuffer<MSG>();
    if (msg == 0)
    {
        // b28c-L20: ORIG 0x0815d2c8 用 __PRETTY_FUNCTION__（P139）且
        // cMyTrace 为全表达式临时链（P221/P227），非具名对象。
        cMyTrace(__PRETTY_FUNCTION__, 0x36e, 0)("[Taiwan, Event] msg is null");
    }
    else
    {
        msg->m_accId = user->get_acc_id();
        msg->m_characNo = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}
