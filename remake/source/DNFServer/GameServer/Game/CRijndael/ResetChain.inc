int CRijndael::ResetChain()
{
    if (!m_bInit)
    {
        return 0x70000005;
    }
    std::memcpy(m_chain, m_block, m_blockSize);
    return 0x6fffffff;
