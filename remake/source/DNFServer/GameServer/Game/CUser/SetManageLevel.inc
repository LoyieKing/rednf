void CUser::SetManageLevel(int level)
{
    if (G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax < level)
        m_field8ebd0 = G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax;
    else
        m_field8ebd0 = level;
