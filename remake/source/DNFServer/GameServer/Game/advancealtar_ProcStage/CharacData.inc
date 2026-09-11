// ----------------------------------------------------------------------------
// ORIG T 0x0812d024（CharacData.md 全文）：4 标量清零 +
//   map<unsigned char,_SlotItem>@+0x10 默认构造（m_slotItems 成员初始化即一致）。
// ----------------------------------------------------------------------------
CharacData::CharacData()
    : m_field0(0), m_user(0), m_gageLevel(0), m_gage(0), m_slotItems()
{
}