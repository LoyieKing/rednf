int Timer_RefreshServerState::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863b90e: ARAD::Arad_ServerStateManager::checkContinus(s_serverStateManager_); return 1
    GlobalData::s_serverStateManager_->checkContinus();
    return 1;
