int CSkillChanger::GetCurCharacTotalSP(const CUser* user, bool flag)
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
    int base = _GetBaseSkillPoint(user);
    _GetUserItemCount(user);
    int quest = _GetQuestSkillPoint(user);
    return base + quest;
