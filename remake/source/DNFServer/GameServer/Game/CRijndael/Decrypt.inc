int CRijndael::Decrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % (size_t)m_blockSize != 0)
        return 0x7000000a;

    int blocks = (int)(iLength / m_blockSize);
    if (m_mode == 1)   // CBC
    {
        for (int i = 0; i < blocks; ++i)
        {
            DecryptBlock(szIn, szOut);
            IMethod::Xor(szOut, (const char*)m_chain);
            std::memcpy(m_chain, szIn, m_blockSize);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    else if (m_mode == 2)   // CFB
    {
        for (int i = 0; i < blocks; ++i)
        {
            EncryptBlock((const char*)m_chain, szOut);
            IMethod::Xor(szOut, szIn);
            std::memcpy(m_chain, szIn, m_blockSize);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    else   // ECB
    {
        for (int i = 0; i < blocks; ++i)
        {
            DecryptBlock(szIn, szOut);
            szIn += m_blockSize;
            szOut += m_blockSize;
        }
    }
    return 0x6fffffff;
