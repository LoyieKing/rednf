CNCryptoRc6::CNCryptoRc6()
    : ICryptoGraph(CRYPTO_GRAPH_RC6)
{
    m_pCrypto = 0;
    RC6_ALG_INFO* c = new RC6_ALG_INFO;
    m_pCrypto = c;
    if (c)
    {
        unsigned char key[0x3c];
        GenerateRandomKey(key, 0x3c);
        Initialize(key, 0x3c);
    }
