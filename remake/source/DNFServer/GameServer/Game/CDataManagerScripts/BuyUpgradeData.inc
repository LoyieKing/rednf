// ORIG _ZN12advancealtar14BuyUpgradeDataC1Ev 0x088a3570 T：构造 string +0xc/
// vector +0x10/map +0x1c 后：+0x00=0、+0x04=0、+0x08=1（ORIG movb 低字节，
// bool 语义）、vector.clear()、map.clear()。
advancealtar::BuyUpgradeData::BuyUpgradeData()
    : m_strDesc(), m_fieldTypes(), m_listMap()
{
    m_nItemId = 0;
    m_field4 = 0;
    m_field8 = 1;
    m_fieldTypes.clear();
    m_listMap.clear();
}