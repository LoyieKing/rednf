int BuyUpgradeData::getFieldType(int idx) const
{
    int result = -1;
    if (idx < (int)m_fieldTypes.size())
    {
        result = (int)m_fieldTypes[idx];
    }
    return result;
