void CPowerManager::SetPowerWarStartTime(unsigned char y, unsigned char m,
                                         unsigned char d, unsigned char h)
{
    m_powerWarStartTime[0] = y;
    m_powerWarStartTime[1] = m;
    m_powerWarStartTime[2] = d;
    m_powerWarStartTime[3] = h;
}