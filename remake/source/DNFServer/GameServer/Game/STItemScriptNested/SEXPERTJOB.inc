// ---- STItemScript::SEXPERTJOB（ORIG 0x89c36aa 族）----
// ORIG C1 0x089c36aa：stExpertCompoundResultVariation@+0 → stExpertJobAdditionalExp@+8
// → vector<SUSABLE>@+0x20（m_field10..m_field1c 不显式初始化，O0 保持 ORIG 原样）。
STItemScript::SEXPERTJOB::SEXPERTJOB()
    : m_variation(), m_additionalExp(), m_usable()
{
}