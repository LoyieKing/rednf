int CTEA::ResetChain()
{
    if (!m_bInit)
    {
        return 0x70000005;
    }
    std::memcpy(m_chain2, m_chain, m_blockSize);
    return 0x6fffffff;
