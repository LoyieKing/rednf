bool CAppLoadChecker::checkLoad_dbq(int cur)
{
    int up = cur - m_baseDbq;     // ORIG -0x8(%ebp)：param − this->+0x00
    int down = m_baseDbq - cur;   // ORIG -0x4(%ebp)

    if (m_stateDbq <= 0 && up > 10) { m_stateDbq = 1; return true; }
    if (m_stateDbq <= 1 && up > 25) { m_stateDbq = 2; return true; }
    if (m_stateDbq <= 2 && up > 40) { m_stateDbq = 3; return true; }
    if (m_stateDbq <= 3 && up > 80) { m_stateDbq = 4; return true; }
    if (m_stateDbq <= 4 && up > 150) { m_stateDbq = 5; return true; }
    if (m_stateDbq <= 5 && up > 250) { m_stateDbq = 6; return true; }
    if (m_stateDbq == 6 && up > 250) return true;

    if (m_stateDbq >= 0 && down > 10) { m_stateDbq = -1; return true; }
    if (m_stateDbq >= -1 && down > 25) { m_stateDbq = -2; return true; }
    if (m_stateDbq >= -2 && down > 40) { m_stateDbq = -3; return true; }
    if (m_stateDbq >= -3 && down > 80) { m_stateDbq = -4; return true; }
    if (m_stateDbq >= -4 && down > 150) { m_stateDbq = -5; return true; }
    if (m_stateDbq >= -5 && down > 250) { m_stateDbq = -6; return true; }
    if (m_stateDbq == -6 && down > 250) return true;
    return false;
