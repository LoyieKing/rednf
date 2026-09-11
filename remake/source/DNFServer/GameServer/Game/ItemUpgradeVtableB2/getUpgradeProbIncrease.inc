// ============================================================================
// CEquipItem::getUpgradeProbIncrease @ 0x0854b552（W，14 字节访问器）
//   return *(int*)((char*)this + 0x1d0)（CEquipItem::m_field1d0）。
// ============================================================================
int CEquipItem::getUpgradeProbIncrease() const
{
    return m_field1d0;
}