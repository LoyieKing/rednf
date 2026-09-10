int CharacAdvanceAltarManager::getCurrentRidableId() const
{
    if (m_currentAdvanceAltarData == 0)
        return 0;
    return m_currentAdvanceAltarData->m_ridableId;
