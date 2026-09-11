// ORIG GetPvpCurrRankPoint 0x85d4880：idx >= 0x26 → +0x12c(=m_data[0x25].curr)；否则 m_data[idx].curr。
int RefPvpGrade::GetPvpCurrRankPoint(int idx) const
{
    if (idx >= 0x26)
    {
        return m_data[0x25].curr; // +0x12c
    }
    return m_data[idx].curr;
}