// ORIG T 0x081316a0：检查首个 AdvanceAltarData 的更新标志（manager+0xd0 == m_data.m_update）。
bool CharacAdvanceAltarManager::isUpdated() const
{
    for (int local_8 = 0; local_8 <= 0; ++local_8)
    {
        if (m_data[local_8].m_update != 0)
            return true;
    }
    return false;
}