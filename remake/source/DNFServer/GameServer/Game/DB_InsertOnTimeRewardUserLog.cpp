// DB_InsertOnTimeRewardUserLog.cpp —— ORIG @0x08442b7a 忠实转写。
#include "DB_InsertOnTimeRewardUserLog.h"

#include "GlobalData.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"

void DB_InsertOnTimeRewardUserLog::makeRequest(int uid, unsigned int accId,
                                               int ontimeId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa445), true);

    guard.operator*()->operator<<(0x298);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(ontimeId);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
