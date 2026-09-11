// InitScript 0x0832b36c：m_endurance = script->m_enduranceInit（+0x274，
// [endurance initial value]）；m_enchantCount = 0；清 30 字节 recipe flag；
// alter()。
bool CCharacExpertJob::InitScript(STExpertJobScript* script)
{
    m_endurance = script->m_enduranceInit;
    m_enchantCount = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
    alter();
    return 1;
}