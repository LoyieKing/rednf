bool CKillMonsterInfo::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("KillMonsterInfo.cpp", 0x6b),
        true);
    
    guard.operator*()->operator<<(0x281);
    guard.operator*()->operator<<((int)pUser->GetUID());

    SIG_LOAD_KILL_MONSTER_INFO* pSig =
        guard->GetInBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    if (pSig == 0)
        return false;
    memset(pSig, 0, 0xafcc);
    pSig->m_characNo = pUser->get_charac_no(-1);
    ConvertMapToStrcut(m_bossKills, (stMonsterInfo_t*)pSig, 0x5dc);
    ConvertMapToStrcut(m_namedKills,
                       pSig->m_named, 6000);
    ConvertMapToStrcut(m_apcBossKills,
                       pSig->m_apc, 0x5dc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
