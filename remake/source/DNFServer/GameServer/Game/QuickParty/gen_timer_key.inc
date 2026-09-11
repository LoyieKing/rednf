// gen_timer_key @0x08269680：计数数组位于 +0x0c 起、槽下标 (msg-0x9d)+4；
// 先自增再返回新值。msg=0x9e 对应 +0x20 槽。
int CQuickParty::gen_timer_key(TIMER_MESSAGE msg)
{
    // ORIG：计数数组 @+0x1c，槽下标 (int)msg-0x9d，先自增再返回新值
    return ++m_timerKeyCnt[static_cast<int>(msg) - 0x9d];
}