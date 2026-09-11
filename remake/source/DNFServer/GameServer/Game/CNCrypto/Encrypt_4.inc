int CNCryptoTea::Encrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len < 1)
        return 0x7000000b;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Encrypt((const char*)buf, (char*)buf, len);
}