int CNCryptoRijndael::Encrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Encrypt((const char*)buf, (char*)buf, len);
