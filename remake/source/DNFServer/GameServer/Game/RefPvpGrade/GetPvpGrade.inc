// ORIG GetPvpGrade 0x85d47ce：idx < this+0xc(=m_data[1].curr) → 0；
// 循环 i=1..<GetLastGrade()：m_data[i].curr < idx && m_data[i].next >= idx → i；否则 -1。
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
}