int CNCryptoRijndael::SetMode(CRYPTO_MODE_TYPE mode)
{
    if (!m_pCrypto)
        return 0x7000000c;
    // ORIG 0x0809c129..0x0809c187（逐分支内联立即数实参、无中间槽；三 call 后
    // 直接 jmp leave ⇒ **返回值 = IMethod::SetMode 的 eax，非固定码**；cmp 序
    // 1,3,0；其余 mode ⇒ 0x70000017）：
    if (mode == 1)
        return ((IMethod*)m_pCrypto)->SetMode(1);
    if (mode == 3)
        return ((IMethod*)m_pCrypto)->SetMode(2);
    if (mode == 0)
        return ((IMethod*)m_pCrypto)->SetMode(0);
    return 0x70000017;
}