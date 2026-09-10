CSHA::CSHA()
{
    for (int i = 0; i < 8; ++i) {
        m_state[i] = sm_H256[i];
    }
    m_lenLo = 0;
    m_lenHi = 0;
    std::memset(m_buffer, 0, sizeof(m_buffer));
    m_pending = 0;
