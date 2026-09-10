bool CInGameAdvertisementManager::ReloadADExposeInfo()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "InGameAdvertisementManager.cpp", 0x150), true);
    guard.operator*()->operator<<(0x2f9);
    guard.operator*()->operator<<(-1);
    SIG_RELOAD_IN_GAME_ADVERTISEMENT* sig =
        guard->GetInBuffer<SIG_RELOAD_IN_GAME_ADVERTISEMENT>();
    if (queryADExposeInfo(
            reinterpret_cast<stInGameADExposeCondition*>(sig)))
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return true;
    }
    return false;
