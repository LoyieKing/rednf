// ORIG GetPvpPoint 0x85d4832：idx < 1 → 0；idx >= 0x26 → +0x12c(=m_data[0x25].curr)；
// 否则 this+idx*8+4(=m_data[idx].curr)。
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
}