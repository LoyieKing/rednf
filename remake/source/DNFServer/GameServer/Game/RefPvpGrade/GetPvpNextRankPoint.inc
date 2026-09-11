// ORIG GetPvpNextRankPoint 0x85d4860：idx >= 0x26 → +0x130(=m_data[0x25].next)；否则 m_data[idx].next。
int RefPvpGrade::GetPvpNextRankPoint(int idx) const
{
    if (idx >= 0x26)
    {
        return m_data[0x25].next; // +0x130
    }
    return m_data[idx].next;
}