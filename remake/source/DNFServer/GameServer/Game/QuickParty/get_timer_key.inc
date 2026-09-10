int CQuickParty::get_timer_key(TIMER_MESSAGE msg)
{
    return m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
