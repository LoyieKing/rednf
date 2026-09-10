int CNCryptoTea::Decrypt(const unsigned char* in, int inLen,
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000b;
    // ORIG 0x0809d2b6：4 参 Decrypt 无 GetBlockSize/模检查，直接虚调用 +0x10。
    return ((CTEA*)m_pCrypto)->Decrypt((const char*)in, (char*)out, inLen);
