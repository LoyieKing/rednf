bool CAppLoadChecker::checkLoad_logdbq(int cur)
{
    int up = cur - m_baseLogdbq;     // ORIG -0x8(%ebp)：param − this->+0x04
    int down = m_baseLogdbq - cur;   // ORIG -0x4(%ebp)

    if (m_stateLogdbq <= 0 && up > 10) { m_stateLogdbq = 1; return true; }
    if (m_stateLogdbq <= 1 && up > 25) { m_stateLogdbq = 2; return true; }
    if (m_stateLogdbq <= 2 && up > 40) { m_stateLogdbq = 3; return true; }
    if (m_stateLogdbq <= 3 && up > 80) { m_stateLogdbq = 4; return true; }
    if (m_stateLogdbq <= 4 && up > 150) { m_stateLogdbq = 5; return true; }
    if (m_stateLogdbq <= 5 && up > 250) { m_stateLogdbq = 6; return true; }
    if (m_stateLogdbq == 6 && up > 250) return true;

    if (m_stateLogdbq >= 0 && down > 10) { m_stateLogdbq = -1; return true; }
    if (m_stateLogdbq >= -1 && down > 25) { m_stateLogdbq = -2; return true; }
    if (m_stateLogdbq >= -2 && down > 40) { m_stateLogdbq = -3; return true; }
    if (m_stateLogdbq >= -3 && down > 80) { m_stateLogdbq = -4; return true; }
    if (m_stateLogdbq >= -4 && down > 150) { m_stateLogdbq = -5; return true; }
    if (m_stateLogdbq >= -5 && down > 250) { m_stateLogdbq = -6; return true; }
    if (m_stateLogdbq == -6 && down > 250) return true;
    return false;
