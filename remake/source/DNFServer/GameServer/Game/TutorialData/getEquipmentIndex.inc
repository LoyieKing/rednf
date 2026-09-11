// ORIG _ZN12TutorialData17getEquipmentIndexE18ENUM_EQUIPMENTTYPEi @0x08ab87be
int TutorialData::getEquipmentIndex(ENUM_EQUIPMENTTYPE type, int slot)
{
    if (slot >= 4) {
        return -1;
    }
    std::map<int, int>::iterator it =
        m_equipmentMaps[slot].find(static_cast<int>(type));
    if (it != m_equipmentMaps[slot].end()) {
        return it->second;
    }
    return -1;
}