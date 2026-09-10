int RefPvpGrade::GetPvpPoint(int idx) const
{
    if (idx <= 0)
    {
        return 0;
    }
    if (idx >= 0x26)
    {
        return m_data[0x25].curr; // +0x12c
    }
    return m_data[idx].curr;
