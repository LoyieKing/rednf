int CSHA::AddData(char const* data, int len)
{
    if (len < 1) {
        return (int)0x70000007;    // ORIG：len <= 0 错误码
    }
    unsigned int prevBits = m_lenLo;
    m_lenLo = (unsigned int)len * 8u + prevBits;
    if (m_lenLo < prevBits) {
        m_lenHi = m_lenHi + 1u;
    }
    m_lenHi = m_lenHi + ((unsigned int)len >> 29u);
    unsigned int pos = (prevBits >> 3u) & 0x3fu;
    if (pos != 0u) {
        unsigned char* tail = m_buffer + pos;
        unsigned int room = 0x40u - pos;
        if ((unsigned int)len < room) {
            std::memcpy(tail, data, len);
            return (int)0x6fffffff;
        }
        std::memcpy(tail, data, room);
        Transform();
        data += room;
        len -= room;
    }
    for (; len > 0x3f; data += 0x40, len -= 0x40) {
        std::memcpy(m_buffer, data, 0x40);
        Transform();
    }
    std::memcpy(m_buffer, data, len);
    m_pending = 1;
    return (int)0x6fffffff;
