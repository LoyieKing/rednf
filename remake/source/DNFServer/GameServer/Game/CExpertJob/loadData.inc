// loadData 0x0832b1fe：pData 即 DB 侧 SIG_EXPERT_JOB 载荷（wire 布局见
// SigTypes.h），按具名字段拷入。
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
}