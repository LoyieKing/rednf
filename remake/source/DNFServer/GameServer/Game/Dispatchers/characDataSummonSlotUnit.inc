int characDataSummonSlotUnit(advancealtar::CharacData* cd, unsigned char slotType, int unitId)
{
    if (cd == 0)
    {
        return 3;
    }
    std::map<unsigned char, advancealtar::_SlotItem>::iterator it =
        cd->m_slotItems.find(slotType);
    if (it == cd->m_slotItems.end())
    {
        return 4;
    }
    const advancealtar::_SlotItem& slotItem = it->second;
    int base = *reinterpret_cast<int*>(G_CDataManager()->m_altarConfig.m_strPtr);
    int cost = base * slotItem.m_field8;
    int gage = cd->m_gage;
    if (slotItem.m_needGage == unitId && cost <= gage)
    {
        cd->m_gage = gage - cost;
        return 0;
    }
    return 2;
