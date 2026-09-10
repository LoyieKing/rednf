bool CharacAdvanceAltarManager::isUpdated() const
{
    for (int local_8 = 0; local_8 <= 0; ++local_8)
    {
        if (m_data[local_8].m_update != 0)
            return true;
    }
    return false;
