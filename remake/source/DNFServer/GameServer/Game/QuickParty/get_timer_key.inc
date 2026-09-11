// ---- get_timer_key @0x08269668：读 timer key 计数数组槽（与 gen_timer_key 同址）----
int CQuickParty::get_timer_key(TIMER_MESSAGE msg)
{
    return m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
}