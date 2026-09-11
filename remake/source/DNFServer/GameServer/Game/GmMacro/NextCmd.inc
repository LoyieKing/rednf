void CMacro::NextCmd()                                  // 0x084a86fe
{
    if (m_curIdx >= m_data.size())
    {
        m_state = 0;
        m_curIdx = 0;
        return;
    }
    stMacroData& cur = m_data[m_curIdx];
    if (cur.m_data != 0)
    {
        SendPacket(cur.m_data, cur.m_len);
    }
    if (static_cast<unsigned int>(m_curIdx + 1) < m_data.size())
    {
        ++m_curIdx;
        if (m_data[m_curIdx].m_data != 0)
        {
            NextCmd();                          // ORIG：新槽 data 非空才尾递归
        }
    }
}