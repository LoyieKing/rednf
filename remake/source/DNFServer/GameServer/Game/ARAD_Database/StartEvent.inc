void CCoinEventOnCharCreate::StartEvent()            // T 0x0810a988
{
    SetEventFlag(true);
    m_coin = 5;
    cMyTrace trace(__FUNCTION__, 0x15, 0);           // __FUNCTION__@0x8b4ac44
    trace("Coin Event On Char Create Start!");       // @0x8b4a4a0
}