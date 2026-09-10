void TutorialData::addEquipmentIndex(int type, int index, int slot)
{
    if (slot >= 4) {
        return;
    }
    m_equipmentMaps[slot].insert(std::make_pair(type, index));
