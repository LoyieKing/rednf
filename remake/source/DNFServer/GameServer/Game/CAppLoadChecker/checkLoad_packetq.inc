bool CAppLoadChecker::checkLoad_packetq(int cur)
{
    int up = cur - m_basePacketq;     // ORIG -0x8(%ebp)：param − this->+0x0c
    int down = m_basePacketq - cur;   // ORIG -0x4(%ebp)

    if (m_statePacketq <= 0 && up > 10) { m_statePacketq = 1; return true; }
    if (m_statePacketq <= 1 && up > 25) { m_statePacketq = 2; return true; }
    if (m_statePacketq <= 2 && up > 40) { m_statePacketq = 3; return true; }
    if (m_statePacketq <= 3 && up > 80) { m_statePacketq = 4; return true; }
    if (m_statePacketq <= 4 && up > 150) { m_statePacketq = 5; return true; }
    if (m_statePacketq <= 5 && up > 250) { m_statePacketq = 6; return true; }
    if (m_statePacketq == 6 && up > 250) return true;

    if (m_statePacketq >= 0 && down > 10) { m_statePacketq = -1; return true; }
    if (m_statePacketq >= -1 && down > 25) { m_statePacketq = -2; return true; }
    if (m_statePacketq >= -2 && down > 40) { m_statePacketq = -3; return true; }
    if (m_statePacketq >= -3 && down > 80) { m_statePacketq = -4; return true; }
    if (m_statePacketq >= -4 && down > 150) { m_statePacketq = -5; return true; }
    if (m_statePacketq >= -5 && down > 250) { m_statePacketq = -6; return true; }
    if (m_statePacketq == -6 && down > 250) return true;
    return false;
