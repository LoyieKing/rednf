bool CItem::GetIncreaseStatusIntData(int idx, int& out) const
    // ORIG W 0x08694658：idx<0 或 idx>=size → false；否则 out=at(idx) → true
{
    if (idx < 0 || idx >= (int)m_vec90.size())
        return false;
    out = m_vec90.at(idx);
    return true;
}