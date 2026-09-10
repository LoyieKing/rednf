void DB_MercenaryLogInfo::makeRequest(unsigned int accId, unsigned int characNo,
                                      unsigned int startSec, unsigned int endSec,
                                      char status, char periodIdx, char areaIdx,
                                      int gold, int itemIdx, int rarity)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xadd0), true);
    guard.operator*()->operator<<((int)0x2b8);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(startSec);
    guard.operator*()->operator<<(endSec);
    guard.operator*()->operator<<(status);
    guard.operator*()->operator<<(periodIdx);
    guard.operator*()->operator<<(areaIdx);
    guard.operator*()->operator<<(gold);
    guard.operator*()->operator<<(itemIdx);
    guard.operator*()->operator<<(rarity);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
