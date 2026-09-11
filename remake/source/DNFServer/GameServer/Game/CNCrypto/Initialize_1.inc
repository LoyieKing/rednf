int CNCryptoRijndael::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    return ((CRijndael*)m_pCrypto)->Initialize(
        (const char*)key, keyLen, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
}