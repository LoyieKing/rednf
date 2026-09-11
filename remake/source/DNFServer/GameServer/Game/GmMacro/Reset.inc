void CMacro::Reset()                                    // 0x084a85b4
{
    if (!m_data.empty())
    {
        for (size_t i = 0; i < m_data.size(); ++i)
        {
            if (m_data[i].m_data != 0)                  // ORIG：operator[] 两次取 data 判空
            {
                delete[] m_data[i].m_data;              // ORIG operator delete[]
            }
        }
        m_data.clear();
    }
    m_curIdx = 0;
    m_state = 0;
}