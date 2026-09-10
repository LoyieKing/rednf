int RefPvpGrade::GetPvpNextRankPoint(int idx) const
{
    if (idx >= 0x26)
    {
        return m_data[0x25].next; // +0x130
    }
    return m_data[idx].next;
