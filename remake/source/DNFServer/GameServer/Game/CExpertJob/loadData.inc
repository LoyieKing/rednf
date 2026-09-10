bool CCharacExpertJob::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    const SIG_EXPERT_JOB* sig = (const SIG_EXPERT_JOB*)pData;
    m_giveupCount = sig->m_giveupCount;
    m_endurance = sig->m_endurance;
    m_enchantCount = sig->m_enchantCount;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = sig->m_recipeFlag[i];
    return 1;
