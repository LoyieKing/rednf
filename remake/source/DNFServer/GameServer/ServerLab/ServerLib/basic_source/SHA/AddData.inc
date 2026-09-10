void CSHA::AddData(const char* pcData, int iDataLength)
{
    if (iDataLength < 1)
    {
        throw CCryptorException("AddData", 0x28, "FileCrypt ERROR: in CSHA::AddData(), Data Length should be > 0!");
    }

    uint uiT = m_auiBits[0];
    m_auiBits[0] = iDataLength * 8 + uiT;
    register bool bCarry = (m_auiBits[0] < uiT);
    if (bCarry)
    {
        m_auiBits[1] = m_auiBits[1] + 1;
    }
    m_auiBits[1] = m_auiBits[1] + (iDataLength >> 0x1d);

    uiT = (uiT >> 3) & 0x3f;
    if (uiT != 0)
    {
        unsigned char* pDest = m_aucIn + uiT;
        uiT = 0x40 - uiT;
        memcpy(pDest, pcData, uiT);
        Transform();
        pcData = pcData + uiT;
        iDataLength = iDataLength - uiT;
    }
    for (; iDataLength > 0x3f; iDataLength = iDataLength - 0x40)
    {
        memcpy(m_aucIn, pcData, 0x40);
        Transform();
        pcData = pcData + 0x40;
    }
    memcpy(m_aucIn, pcData, iDataLength);
    m_bAddData = true;
