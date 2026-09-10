int CRijndael::Initialize(const char* key, int keyLen, const char* chain,
                          int chainLen, int blockSize, int mode, int padding)
{
    if (key == 0)
        return 0x70000001;
    if (keyLen < 1)
        return 0x70000002;
    if (chainLen != 0x10 && chainLen != 0x18 && chainLen != 0x20)
        return 0x70000009;
    if (blockSize != 0x10 && blockSize != 0x18 && blockSize != 0x20)
        return 0x70000006;
    if (mode < 0 || mode > 2)
        return 0x70000003;
    if (padding < 0 || padding > 2)
        return 0x70000004;

    m_mode = mode;
    m_padding = padding;

    // key 循环填充到 chainLen 字节（ORIG local_58）
    unsigned char cycled[0x20];
    int src = 0;
    for (int i = 0; i < chainLen; ++i)
    {
        cycled[i] = (unsigned char)key[src];
        src = (src + 1) % keyLen;
    }

    m_blockSize = blockSize;
    std::memcpy(m_block, chain, blockSize);
    std::memcpy(m_chain, m_block, blockSize);
    m_keyLength = chainLen;
    std::memcpy(m_key, cycled, m_keyLength);

    int nk = m_keyLength / 4;
    int nb = m_blockSize / 4;
    if (nk == 4)
        m_rounds = (nb == 4) ? 10 : (nb == 6) ? 12 : 14;
    else if (nk == 6)
        m_rounds = (nb == 8) ? 14 : 12;
    else
        m_rounds = 14;

    KeyExpansion();
    m_bInit = true;
    return 0x6fffffff;
