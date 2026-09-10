bool CAppLoadChecker::checkLoad_timerq(int cur)
{
    int up = cur - m_baseTimerq;     // ORIG -0x8(%ebp)：param − this->+0x08
    int down = m_baseTimerq - cur;   // ORIG -0x4(%ebp)

    if (m_stateTimerq <= 0 && up > 10) { m_stateTimerq = 1; return true; }
    if (m_stateTimerq <= 1 && up > 25) { m_stateTimerq = 2; return true; }
    if (m_stateTimerq <= 2 && up > 40) { m_stateTimerq = 3; return true; }
    if (m_stateTimerq <= 3 && up > 80) { m_stateTimerq = 4; return true; }
    if (m_stateTimerq <= 4 && up > 150) { m_stateTimerq = 5; return true; }
    if (m_stateTimerq <= 5 && up > 250) { m_stateTimerq = 6; return true; }
    if (m_stateTimerq == 6 && up > 250) return true;

    if (m_stateTimerq >= 0 && down > 10) { m_stateTimerq = -1; return true; }
    if (m_stateTimerq >= -1 && down > 25) { m_stateTimerq = -2; return true; }
    if (m_stateTimerq >= -2 && down > 40) { m_stateTimerq = -3; return true; }
    if (m_stateTimerq >= -3 && down > 80) { m_stateTimerq = -4; return true; }
    if (m_stateTimerq >= -4 && down > 150) { m_stateTimerq = -5; return true; }
    if (m_stateTimerq >= -5 && down > 250) { m_stateTimerq = -6; return true; }
    if (m_stateTimerq == -6 && down > 250) return true;
    return false;
