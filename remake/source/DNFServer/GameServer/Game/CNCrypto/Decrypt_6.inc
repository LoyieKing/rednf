int CNCryptoBlowFish::Decrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    if (m_mode == 1)
        return ((CBlowFish*)m_pCrypto)->Decrypt(buf, buf, len, 1);
    if (m_mode == 3)
        return ((CBlowFish*)m_pCrypto)->Decrypt(buf, buf, len, 2);   // ORIG 2 参版 mode3 传 2
    if (m_mode == 0)
        return ((CBlowFish*)m_pCrypto)->Decrypt(buf, buf, len, 0);
    return 0x70000017;
