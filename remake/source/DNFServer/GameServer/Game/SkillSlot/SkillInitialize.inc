void CSkillChanger::SkillInitialize(CUser* user, int kind, bool flag)
{
    m_state.m_initialized = flag;
    memset(&m_state.m_sec0, 0, 0x24);
    memset(&m_state.m_sec1, 0, 0x24);
    memset(&m_state.m_sec2, 0, 0x24);
    memset(&m_state.m_sec3, 0, 0x24);
    m_state.m_sec0.m_kindState = 8;
    m_state.m_sec1.m_kindState = 8;
    m_state.m_sec2.m_kindState = 8;
    m_state.m_sec3.m_kindState = 8;
    m_state.m_reserved91 = 0;
    if (kind == 0)
    {
        _ResetSkillPoint(user, (ENUM_SKILL_TREE_KIND)0);
        _ResetSkillPoint(user, (ENUM_SKILL_TREE_KIND)1);
        _ResetSFPoint(user, (ENUM_SKILL_TREE_KIND)2);
        _ResetSFPoint(user, (ENUM_SKILL_TREE_KIND)3);
        _GetUserItemCount(user);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)0);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)1);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)2);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)3);
    }
    else if (kind == 1)
    {
        _ResetSkillPoint(user, (ENUM_SKILL_TREE_KIND)0);
        _ResetSFPoint(user, (ENUM_SKILL_TREE_KIND)2);
        _GetUserItemCount(user);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)0);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)2);
    }
    else if (kind == 2)
    {
        _ResetSkillPoint(user, (ENUM_SKILL_TREE_KIND)1);
        _ResetSFPoint(user, (ENUM_SKILL_TREE_KIND)3);
        _GetUserItemCount(user);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)1);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)3);
    }
    else if (kind == 3)
    {
        _ResetSFPoint(user, (ENUM_SKILL_TREE_KIND)2);
        _GetUserItemCount(user);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)2);
    }
    else if (kind == 4)
    {
        _ResetSFPoint(user, (ENUM_SKILL_TREE_KIND)3);
        _GetUserItemCount(user);
        _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)3);
    }
