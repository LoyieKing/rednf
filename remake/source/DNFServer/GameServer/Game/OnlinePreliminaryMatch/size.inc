int TeamInfoSecond::size() const
{
    int n = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_seats[i].m_flag != 0)
            ++n;
    }
    return n;
