// ORIG 0x851547e：+0x1e0 WpBonusPointData 与 +0x1e8 ExpBonusPointData 各自
// clear()（m_type=2、m_value=0）。本文件以 4 个 int 平铺该 16 字节区
// （m_field1e0..m_field1ec），初始化列表按声明顺序（0x1f0 vector 之前）补齐。
CEquipItem::CEquipItem()
    : m_wpBonus1e0(),   // WpBonusPointData::C1（type=2, value=0）
      m_expBonus1e8()    // ExpBonusPointData::C1（type=2, value=0）
{
}