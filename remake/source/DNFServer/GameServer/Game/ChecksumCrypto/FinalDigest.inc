int CSHA::FinalDigest(char* out)
{
    if (m_pending != 1) {
        return (int)0x70000008;    // ORIG：缓冲区有残留时拒绝 finalize
    }
    unsigned int pos = (m_lenLo >> 3u) & 0x3fu;
    unsigned char* tail = m_buffer + pos;
    *tail = 0x80;
    ++tail;
    unsigned int room = 0x3fu - pos;
    if (room < 8u) {
        std::memset(tail, 0, room);
        Transform();
        std::memset(m_buffer, 0, 0x38);
    } else {
        std::memset(tail, 0, room - 8u);
    }
    Word2Bytes(m_lenHi, m_buffer + 0x38);
    Word2Bytes(m_lenLo, m_buffer + 0x3c);
    Transform();
    for (int i = 0; i < 8; ++i, out += 4) {
        Word2Bytes(m_state[i], reinterpret_cast<unsigned char*>(out));
    }
    Reset();
    return (int)0x6fffffff;
}