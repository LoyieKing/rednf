int CNCryptoBlowFish::Encrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    // ORIG 0x08099b2a：按 m_mode 分派到 CBlowFish::Encrypt(in,out,len,mode)；
    // mode 1 与 3 同传 1，mode 0 传 0，其余 0x70000017。
    if (m_mode == 1)
        return ((CBlowFish*)m_pCrypto)->Encrypt(buf, buf, len, 1);
    if (m_mode == 3)
        return ((CBlowFish*)m_pCrypto)->Encrypt(buf, buf, len, 2);   // ORIG 2 参版 mode3 传 2
    if (m_mode == 0)
        return ((CBlowFish*)m_pCrypto)->Encrypt(buf, buf, len, 0);
    return 0x70000017;
}