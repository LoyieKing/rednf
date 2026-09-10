void CSHA::FinalDigest(char* pcDigest)
{
    if (m_bAddData != true)
    {
        throw CCryptorException("FinalDigest", 0x54, "FileCrypt ERROR: in CSHA::FinalDigest(), No data Added before call!");
    }

    uint uiCount = (m_auiBits[0] >> 3) & 0x3f;
    unsigned char* puc = m_aucIn + uiCount;
    *puc = 0x80;
    puc = puc + 1;
    uiCount = 0x3f - uiCount;
    if (uiCount < 8)
    {
        memset(puc, 0, uiCount);
        Transform();
        memset(m_aucIn, 0, 0x38);
    }
    else
    {
        memset(puc, 0, uiCount - 8);
    }
    Word2Bytes(m_auiBits[1], m_aucIn + 0x38);
    Word2Bytes(m_auiBits[0], m_aucIn + 0x3c);
    Transform();
    // ORIG：while 形（体内 i++ 先于 pcDigest += 4）。
    for (int i = 0; i < 8; )
    {
        Word2Bytes(m_auiBuf[i], (unsigned char*)pcDigest);
        i++;
        pcDigest = pcDigest + 4;
    }
    Reset();
