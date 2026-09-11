void PacketBuf::copy(const PacketBuf& other)
{
    m_use_size = other.m_use_size;
    m_size = other.m_size;
    m_finalized = other.m_finalized;
    m_ptr = m_buf;
    memset(m_buf, 0, MAX_PACKET_SIZE);
    memcpy(m_buf, other.m_buf, m_size);
}