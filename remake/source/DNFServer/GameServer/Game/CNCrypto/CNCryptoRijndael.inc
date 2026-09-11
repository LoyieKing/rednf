CNCryptoRijndael::CNCryptoRijndael()
    : ICryptoGraph(CRYPTO_GRAPH_RIJNDAEL)
{
    m_pCrypto = 0;
    m_pCrypto = new CRijndael;
    if (m_pCrypto != 0)
    {
        char key[0x80];
        GenerateRandomString(key, 0x80);
        if (((CRijndael*)m_pCrypto)->Initialize(key, 0x80, CRijndael::sm_chain0,
                                                0x10, 0x10, 0, 0) != 0x6fffffff)
        {
            delete (CRijndael*)m_pCrypto;
            m_pCrypto = 0;
        }
    }
}