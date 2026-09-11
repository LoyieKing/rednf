// ---- stItemMakingSkill（ORIG C1 0x8370ff2 / clear 0x837105a / D1 0x837dce2）----
stItemMakingSkill::stItemMakingSkill() : m_map0(), m_vec18()
{
    clear();   // ORIG C1：构造 map/vector 后调 clear()（清三 int）
}