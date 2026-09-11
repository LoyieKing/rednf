// 0x0891bb5e：pair/string/vector/map 按声明序默认构造；函数体赋默认值
//   （+0x00=0、pair{-1,-1}、+0x0c=-1、+0x10=-1、string=""、vector.clear()）。
PremiumServiceData::PremiumServiceData()
{
    m_type = 0;
    m_mainImageIndex.first = -1;
    m_mainImageIndex.second = -1;
    m_additionalImageIndex = -1;
    m_iconIndex = -1;
    m_iconExplain = "";
    m_addEquipmentList.clear();
}