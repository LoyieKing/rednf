void RefPvpGrade::Clear()
{
    m_lastGrade = 0;
    for (int i = 0; i < 0x26; ++i)
    {
        m_data[i].curr = 0;
        m_data[i].next = 0;
    }
