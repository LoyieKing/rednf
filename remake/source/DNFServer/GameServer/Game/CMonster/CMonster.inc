CMonster::CMonster()
{
    // ORIG 0x08379ea4：仅构造 vector 成员与 m_script，标量不初始化
    // （值由 set_monster 填充；m_index/m_specialItemWeight 等保持未定值）。
}