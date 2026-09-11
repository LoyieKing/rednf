// ORIG Clear 0x85d4722：this+0 = 0；循环 i=0..0x25：this+4+i*8=0、this+8+i*8=0。
void RefPvpGrade::Clear()
{
    m_lastGrade = 0;
    for (int i = 0; i < 0x26; ++i)
    {
        m_data[i].curr = 0;
        m_data[i].next = 0;
    }
}