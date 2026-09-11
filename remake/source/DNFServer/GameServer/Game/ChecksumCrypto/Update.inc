void CSHA1::Update(unsigned char const* data, unsigned int len)
{
    // ORIG 0x0808a19e：位计数（含进位），缓冲区满则 Transform（整块 0x40）。
    unsigned int buffered = (m_countLo >> 3) & 0x3f;
    m_countLo += len * 8u;
    if (m_countLo < len * 8u)
        m_countHi += 1;
    m_countHi += (len >> 0x1d);

    unsigned int copied = 0;
    if (buffered + len >= 0x40)
    {
        copied = 0x40 - buffered;
        memcpy(&m_buffer[buffered], data, copied);
        Transform(m_state, m_buffer);
        for (; copied + 0x3f < len; copied += 0x40)
            Transform(m_state, data + copied);
        buffered = 0;
    }
    if (len != copied)
        memcpy(&m_buffer[buffered], data + copied, len - copied);
}