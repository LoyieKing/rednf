// ORIG _ZN12TutorialData17addEquipmentIndexEiii @0x08ab86e0：
// p3<4 门控；make_pair(p1,p2) 插入 m_equipmentMaps[p3]。
void TutorialData::addEquipmentIndex(int type, int index, int slot)
{
    if (slot >= 4) {
        return;
    }
    m_equipmentMaps[slot].insert(std::make_pair(type, index));
}