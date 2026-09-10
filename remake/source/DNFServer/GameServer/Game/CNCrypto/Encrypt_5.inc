int CNCryptoTea::Encrypt(const unsigned char* in, int inLen,
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000b;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Encrypt((const char*)in, (char*)out, inLen);
