// ---- ORIG _ZN4CTEA7EncryptEPKcPcj（0x080c38e4）：CBC 链式，三模式 ----
int CTEA::Encrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % m_blockSize != 0)
        return 0x7000000a;
    const char* src = szIn;
    char* out = szOut;
    size_t blocks = iLength / m_blockSize;
    for (size_t i = 0; i < blocks; ++i)
    {
        if (m_mode == 1)
        {
            IMethod::Xor((char*)m_chain2, src);                // m_chain2(+0x40)
            EncryptBlock(m_chain2, (unsigned char*)out);
            memcpy(m_chain2, out, m_blockSize);
        }
        else if (m_mode == 2)
        {
            EncryptBlock(m_chain2, (unsigned char*)out);
            IMethod::Xor(out, src);
            memcpy(m_chain2, out, m_blockSize);
        }
        else
        {
            EncryptBlock((unsigned char const*)src, (unsigned char*)out);
        }
        src += m_blockSize;
        out += m_blockSize;
    }
    return 0x6fffffff;
}