bool CCharacExpertJob::InitScript(STExpertJobScript* script)
{
    m_endurance = script->m_enduranceInit;
    m_enchantCount = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
    alter();
    return 1;
