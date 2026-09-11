// ORIG T 0x088a3012：idx 越界 → -1；否则 m_fieldTypes[idx]。
int BuyUpgradeData::getFieldType(int idx) const
{
    int result = -1;
    if (idx < (int)m_fieldTypes.size())
    {
        result = (int)m_fieldTypes[idx];
    }
    return result;
}