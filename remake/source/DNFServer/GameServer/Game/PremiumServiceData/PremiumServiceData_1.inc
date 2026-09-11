// 0x089317a0：POD 成员（int/pair）直接拷贝，string/vector/map 用拷贝构造。
PremiumServiceData::PremiumServiceData(PremiumServiceData const& rhs)
    : m_type(rhs.m_type),
      m_mainImageIndex(rhs.m_mainImageIndex),
      m_additionalImageIndex(rhs.m_additionalImageIndex),
      m_iconIndex(rhs.m_iconIndex),
      m_iconExplain(rhs.m_iconExplain),
      m_addEquipmentList(rhs.m_addEquipmentList),
      m_selectAbleEquipmentList(rhs.m_selectAbleEquipmentList)
{
}