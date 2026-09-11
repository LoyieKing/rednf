int CDeathTower::_genTimerKey(TIMER_MESSAGE msg)
{
    // ORIG 0x830edec：返回 ++m_timerKeys[msg - 0x48]。
    return ++m_timerKeys[(int)msg - 0x48];
}