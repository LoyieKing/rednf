int CNCryptoBlowFish::Decrypt(const unsigned char* in, int inLen,
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000a;
    if ((inLen & 7) != 0)
        return 0x70000006;
    if (m_mode == 1)
        return ((CBlowFish*)m_pCrypto)->Decrypt(in, out, inLen, 1);
    // [DNF-FIX 2026-10-08] ORIG 0x08099eb7: mode 3 传 2 (movl $0x2,0x10(%esp))
    if (m_mode == 3)
        return ((CBlowFish*)m_pCrypto)->Decrypt(in, out, inLen, 2);
    if (m_mode == 0)
        return ((CBlowFish*)m_pCrypto)->Decrypt(in, out, inLen, 0);
    return 0x70000017;
}