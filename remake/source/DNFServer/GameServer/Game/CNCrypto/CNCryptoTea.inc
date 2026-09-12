CNCryptoTea::CNCryptoTea()
    : ICryptoGraph(CRYPTO_GRAPH_TEA)
{
    m_pCrypto = 0;
    m_pCrypto = new CTEA;
    if (m_pCrypto != 0)
    {
        char key[0x80];
        GenerateRandomString(key, 0x80);
        if (((CTEA*)m_pCrypto)->Initialize(key, 0x80, CTEA::sm_chain0, 0, 0) != 0x6fffffff)
        {
            delete (CTEA*)m_pCrypto;
            m_pCrypto = 0;
        }
    }
}