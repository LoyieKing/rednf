bool CDeathTower::_checkTimerKey(TIMER_MESSAGE msg, unsigned int key)
{
    // ORIG 0x8469c68：key == m_timerKeys[msg - 0x48]。
    return key == (unsigned int)m_timerKeys[(int)msg - 0x48];
