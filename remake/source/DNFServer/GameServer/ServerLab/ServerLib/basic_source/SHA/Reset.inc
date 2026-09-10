void CSHA::Reset()
{
    for (int i = 0; i < 8; i++)
    {
        m_auiBuf[i] = sm_H256[i];
    }
    m_auiBits[0] = 0;
    m_auiBits[1] = 0;
    m_bAddData = false;
