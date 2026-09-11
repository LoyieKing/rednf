int CCargo::_IsExistEquipItem(int itemIdx, int slot)
{
    for (int i = 0; i <= 0x97; ++i) {
        if (!m_pItems[i].isEquipableItemType()) continue;
        if (m_pItems[i].m_addInfo != itemIdx) continue;
        if (m_pItems[i].m_addInfo2 != slot) continue;
        return 1;
    }
    return 0;
}