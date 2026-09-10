int Arad_INTER_kSaveServerState::dispatch_sig(CUser* /*user*/, char* body, int len)
{
    const char* packet = body;   // ORIG 用 body(char*@+0x10)；我们误 cast len(int@+0x14)
    if (packet == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x521, "Packet_Arad_ApplyEffect is null.");
        return 0x522;
    }

    AradServerStateMessage msg;                      // ORIG W ctor：全零
    msg.m_active = 1;                                // short @+0x00
    msg.m_endSec = ((const PktSaveServerStateView*)packet)->m_endSec;

    GlobalData::s_serverStateManager_->insertState(
        (ENUM_GLOBAL_EFFECT_ARAD)((const PktSaveServerStateView*)packet)->m_effect, msg);
    return 0;
