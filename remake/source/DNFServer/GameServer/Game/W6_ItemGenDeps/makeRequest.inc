// ORIG T 0x0842894c _ZN27DB_UpdateItemDropStatistics11makeRequestEiiPjS0_S0_S0_
void DB_UpdateItemDropStatistics::makeRequest(int serverId, int typeId,
                                              unsigned int* counts0,
                                              unsigned int* counts1,
                                              unsigned int* counts2,
                                              unsigned int* counts3)
{
    // ORIG 直接把 Acquire 返回值传入守卫构造（无中间局部，栈帧一致）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6a14), true);
    guard.operator*()->operator<<(0xd2);      // SIG_UPDATE_ITEM_DROP_STATISTICS
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(serverId);  // ORIG 实参 = env+0x1b0
                                              //   = m_serverEnvir.m_channelNo
    guard.operator*()->operator<<(typeId);

    // 四组计数各写 6 个 uint（asm: i<=5 循环 cmpl $0x5 + setle）
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts0[i]);
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts1[i]);
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts2[i]);
    for (int i = 0; i <= 5; ++i)
        guard.operator*()->operator<<(counts3[i]);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}