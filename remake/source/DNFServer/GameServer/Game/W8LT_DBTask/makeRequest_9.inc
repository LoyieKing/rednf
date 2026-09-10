void DB_LoadBreakAwayRewardData::makeRequest(
    int userIdx, unsigned int param2, unsigned int param3,
    STBreakAwayRewards& rewards)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x86d3), true);
    guard.operator*()->operator<<((int)0x142);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    guard.operator*()->operator<<((unsigned int)param3);
    // ORIG 0x08439204 `mov (%eax),%ebx` + call _ZN12CStreamGuardlsEj
    // （unsigned int 重载，080e2292）——字段必须走 uint 重载；单条装载要求
    // 字段类型为 unsigned int（CUser.h 现声明 int，已上报 Main 代落）。
    guard.operator*()->operator<<((unsigned int)rewards.m_rewardItemId);
    guard.operator*()->operator<<((int)rewards.m_rewardAItemCnt);
    guard.operator*()->operator<<((int)rewards.m_luckPoint);
    guard.operator*()->operator<<((int)rewards.m_itemOrder);
    guard.operator*()->operator<<((unsigned int)rewards.m_fieldc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
