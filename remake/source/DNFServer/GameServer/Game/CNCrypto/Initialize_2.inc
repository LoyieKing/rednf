int CNCryptoTea::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    return ((CTEA*)m_pCrypto)->Initialize((const char*)key, keyLen, "", 0, 0);
