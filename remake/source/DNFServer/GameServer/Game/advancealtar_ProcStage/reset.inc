// ORIG T 0x0812d078（reset.md 全文）：m_gageLevel/m_user/m_gage/m_field0 清零 +
//   m_slotItems clear。
void CharacData::reset()
{
    m_gageLevel = 0;
    m_user = 0;
    m_gage = 0;
    m_field0 = 0;
    m_slotItems.clear();
}