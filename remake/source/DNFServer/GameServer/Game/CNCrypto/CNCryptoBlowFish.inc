CNCryptoBlowFish::CNCryptoBlowFish()
    : ICryptoGraph(CRYPTO_GRAPH_BLOWFISH)
{
    m_pCrypto = 0;
    m_mode = 0;
    // ORIG 0x0809980c..0x080998f7：GenerateRandomKey(key,0x38) → SBlock chain(0,0)
    // → new CBlowFish(key,0x38,chain) → m_pCrypto=c；m_pCrypto==0 时
    // NCryptoSetLastError(0x7000000d)（0x0809e45c）。
    unsigned char key[0x38];
    GenerateRandomKey(key, 0x38);
    SBlock chain(0, 0);
    CBlowFish* c = new CBlowFish(key, 0x38, chain);
    m_pCrypto = c;
    if (m_pCrypto == 0)
        NCryptoSetLastError(0x7000000d);
}