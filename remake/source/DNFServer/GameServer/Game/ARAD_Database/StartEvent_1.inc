void CCoinEventOnCharCreate::StartEvent(Word_Param param)   // T 0x0810a9e4
{
    SetEventFlag(true);
    m_coin = static_cast<int>(param.m_duration);     // 低 ushort 直接写入 +0xc
    cMyTrace trace(__FUNCTION__, 0x1c, 0);           // __FUNCTION__@0x8b4ac39
    trace("Coin Event On Char Create Start!");
}