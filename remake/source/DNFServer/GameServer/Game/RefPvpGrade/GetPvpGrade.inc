int RefPvpGrade::GetPvpGrade(int idx) const
{
    if (m_data[1].curr > idx)
    {
        return 0;
    }
    for (int i = 1; i < GetLastGrade(); ++i)
    {
        if (m_data[i].curr < idx && m_data[i].next >= idx)
        {
            return i;
        }
    }
    return -1;
