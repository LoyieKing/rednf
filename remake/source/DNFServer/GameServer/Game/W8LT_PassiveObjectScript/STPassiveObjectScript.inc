// ---- STPassiveObjectScript（ORIG C1 T 0x08a3a538 / clear T 0x08a3a9e6）----
// C1：按声明序构造全部非平凡成员（string/vector/子对象，call 序实证）→ clear()；
// 标量由 clear 统一赋值。D1/D2 W 0x08a3e25a 为编译器隐式生成（逆序析构成员），
// 本 TU 不定义（P12）。
STPassiveObjectScript::STPassiveObjectScript()
    : m_name(), m_layer(), m_category(), m_s34(), m_basicActions(), m_s44(),
      m_etcActions(), m_s54(), m_s64(), m_s68(), m_attackInfos(), m_intData(),
      m_stringData(), m_s90(), m_destroy(), m_addEffect(), m_particles(),
      m_s154(), m_onAttackData(), m_s17c(), m_speedCtrls(), m_genPatterns(),
      m_genSequences(), m_genObjects(), m_warnDanger(),
      m_warnRecovery(), m_sniper(), m_itemFairies(), m_s284()
{
    clear();
}