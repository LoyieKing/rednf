// DB_MercenaryLogInfo::makeRequest —— ORIG T 0x08446340 逐指令转写
// （opcode 696(0x2b8) → -1 → accId/characNo/startSec/endSec(uint) →
//  status/periodIdx/areaIdx(char) → gold/itemIdx/rarity(int) → LOGDB_Q(4)）。
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
}