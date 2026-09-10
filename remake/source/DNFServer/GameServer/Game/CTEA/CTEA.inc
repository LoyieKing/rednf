CTEA::CTEA()
{
    m_blockSize = 8;
    m_keyLength = 0x10;
    std::memset(m_key, 0, sizeof(m_key));
    std::memset(m_chain, 0, sizeof(m_chain));
    std::memset(m_chain2, 0, sizeof(m_chain2));
    std::memset(m_iv, 0, sizeof(m_iv));
