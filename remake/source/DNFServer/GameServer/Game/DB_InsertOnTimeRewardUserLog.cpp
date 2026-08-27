// DB_InsertOnTimeRewardUserLog.cpp —— ORIG @0x08442b7a 忠实转写。
#include "DB_InsertOnTimeRewardUserLog.h"

#include "GlobalData.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"

void DB_InsertOnTimeRewardUserLog::makeRequest(int uid, unsigned int accId,
                                               int ontimeId)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa445);
    CStreamGuard guard(stream, true);

    guard << 0x298;
    guard << uid;
    guard << accId;
    guard << ontimeId;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
