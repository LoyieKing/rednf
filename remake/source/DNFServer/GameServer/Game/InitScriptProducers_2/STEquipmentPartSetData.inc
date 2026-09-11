// ============================================================================
// STEquipmentPartSetData —— ORIG C1 0x0891b002 / D1 0x08907348
// ============================================================================
// b28c-L20: AE — ORIG C1 0x0891b002 orig=32 our=32 gap=0 ndiff=0。顺序 = 成员构造（+0x04 string、+0x0c vector）
// 先于标量赋值（+0x00=-1、+0x08=0）⇒ 标量必须写在函数体，不能进初始化列表
// （进列表会被 4.4 交错到成员构造之间，ndiff 19）。
STEquipmentPartSetData::STEquipmentPartSetData()
    : m_name4(), m_pieces()
{
    m_field0 = -1;
    m_script = 0;
}