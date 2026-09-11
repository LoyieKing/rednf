void CCoinEventOnCharCreate::EndEvent()              // T 0x0810aa42
{
    SetEventFlag(false);
    m_coin = 5;
    cMyTrace trace(__FUNCTION__, 0x24, 0);             // __FUNCTION__@0x8b4ac30
    trace("Coin Event On Char Create End!");         // @0x8b4a4c4
}