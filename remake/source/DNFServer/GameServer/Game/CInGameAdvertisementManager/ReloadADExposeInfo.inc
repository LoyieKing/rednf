// ORIG T _ZN27CInGameAdvertisementManager18ReloadADExposeInfoEv @0x080fd95e
//（报告 ReloadADExposeInfo.md）：stream 0x2f9 + (-1) → GetInBuffer 缓冲 →
// queryADExposeInfo 成功则 put(DISPATCH_Q) 广播重载。
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
}