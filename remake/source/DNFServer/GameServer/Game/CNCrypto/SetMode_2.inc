int CNCryptoTea::SetMode(CRYPTO_MODE_TYPE mode)
{
    if (!m_pCrypto)
        return 0x7000000c;
    // ORIG 0x0809d061..0x0809d0bf：与 Rijndael 同构（call IMethod::SetMode、
    // 返回值透传；cmp 序 1,3,0），但**未匹配失败码 = 0x70000003**（0x0809d0ba
    // `mov $0x70000003,%eax`；旧形直写 CTEA::m_mode + 固定 0x6fffffff 属误转写）。
    if (mode == 1)
        return ((IMethod*)m_pCrypto)->SetMode(1);
    if (mode == 3)
        return ((IMethod*)m_pCrypto)->SetMode(2);
    if (mode == 0)
        return ((IMethod*)m_pCrypto)->SetMode(0);
    return 0x70000003;
}