// ORIG T 0x081312d0（getCurrentRidableId.md 全文）：current 空 → 0；
// 否则 *current（AdvanceAltarData::m_ridableId，_RidableBase+0x00）。
int CharacAdvanceAltarManager::getCurrentRidableId() const
{
    if (m_currentAdvanceAltarData == 0)
        return 0;
    return m_currentAdvanceAltarData->m_ridableId;
}