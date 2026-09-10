int CQuickParty::gen_timer_key(TIMER_MESSAGE msg)
{
    // ORIG：计数数组 @+0x1c，槽下标 (int)msg-0x9d，先自增再返回新值
    return ++m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
