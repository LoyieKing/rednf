// 0x0891bc4a：与 C1 相同的默认值，另清空 map。
void PremiumServiceData::clear()
{
    m_type = 0;
    m_mainImageIndex.first = -1;
    m_mainImageIndex.second = -1;
    m_additionalImageIndex = -1;
    m_iconIndex = -1;
    m_iconExplain = "";
    m_addEquipmentList.clear();
    m_selectAbleEquipmentList.clear();
}