void expert_job::CCharacExpertJob::_reset()
{
    m_giveupCount = 0;
    m_endurance = 0;
    m_enchantCount = 0;
    for (int i = 0; i < 0x1e; ++i)
        m_recipeFlag[i] = 0;
