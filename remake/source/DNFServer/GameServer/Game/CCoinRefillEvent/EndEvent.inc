void CCoinRefillEvent::EndEvent()
{
    // ORIG 0x0832c048。
    SetEventFlag(false);
    memset(&m_timeCount, -1, 0x14);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x4f, 0);
    trace("Coin Refill Event End");
}