// ============================================================================
// CEquipItem::getUpgradeCostDiscount @ 0x822ca80
//   返回 +0x1cc 的 float 折扣率（强化成本折扣；ORIG flds 位读）。
// ============================================================================
float CEquipItem::getUpgradeCostDiscount() const
{
    return m_field1cc;   // ORIG flds 0x1cc 位读直返
}