void DB_UpdateDeathTowerBestRecord::makeRequest(
    unsigned int characNo, unsigned int accId, unsigned int memberCount,
    stDeathTowerRecordMemberInfo_t const* members, unsigned int stage, unsigned int playTime)
{
    // ORIG：StreamPool::Acquire(s_stream_pool, "DBThread.cpp", 0x6bfe)
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6bfe), true);
    guard.operator*()->operator<<((int)0xe3);
    guard.operator*()->operator<<((int)-1);
    SIG_DEATH_TOWER_UPDATE_BEST_RECORD* sig =
        guard.operator->()->GetInBuffer<SIG_DEATH_TOWER_UPDATE_BEST_RECORD>();
    sig->m_accId = accId;
    sig->m_towerIndex = static_cast<unsigned short>(characNo);
    sig->m_memberCount = static_cast<unsigned short>(memberCount & 0xff);
    memcpy(sig->m_members, members, memberCount * 0x17);   // ORIG imul 直接用原值
    sig->m_stage = stage;
    sig->m_playTime = playTime;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}