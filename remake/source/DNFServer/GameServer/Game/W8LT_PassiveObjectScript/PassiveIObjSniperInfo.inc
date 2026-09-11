// ---- PassiveIObjSniperInfo（ORIG C1/clear T 0x08a3af0c/0x08a3afc0）----
// C1：4 × vector<int> 构造 → clear()（带 unwind 清理路径）；
// clear：4 × vector::clear → 标量 0x30/0x34/0x38/0x40/0x3c 清零 →
// 循环 i=0..5 清 +0x44..+0x58 → 循环 i=0..2 清 +0x74..+0x7c →
// 0x5c/0x60/0x64/0x6c/0x70 清零（+0x68 不重置）。
PassiveIObjSniperInfo::PassiveIObjSniperInfo()
    : m_vecMinDist(), m_vecMaxDist(), m_vecField18(), m_vecField24()
{
    clear();
}