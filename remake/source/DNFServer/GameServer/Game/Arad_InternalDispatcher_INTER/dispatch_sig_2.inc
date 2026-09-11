// ============================================================================
// Arad_INTER_kDeleteServerState —— ORIG 0x0819935e
// ============================================================================
int Arad_INTER_kDeleteServerState::dispatch_sig(CUser* /*user*/, char* body, int len)
{
    const char* packet = body;   // ORIG 用 body(char*@+0x10)；我们误 cast len(int@+0x14)
    if (packet == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x540, "Packet_Arad_DeleteEffect is null.");
        return 0x541;
    }
    GlobalData::s_serverStateManager_->deleteState(
        (ENUM_GLOBAL_EFFECT_ARAD)((const PktSaveServerStateView*)packet)->m_effect);
    return 0;
}