// ---- ORIG T 0x08131254（setCurrentAdvanceAltarData.md 全文）----
bool CharacAdvanceAltarManager::setCurrentAdvanceAltarData(RidableId::T ridableId)
{
    if (!isValidRidableId(ridableId))
    {
        return false;
    }
    for (int i = 0; i <= 0; ++i)
    {
        if (m_data[i].m_ridableId == ridableId)   // 自然数组寻址：ORIG imul $0xd0 字节乘
        {
            m_currentAdvanceAltarData = &m_data[i];
            return true;
        }
    }
    m_currentAdvanceAltarData = 0;
    return false;
}