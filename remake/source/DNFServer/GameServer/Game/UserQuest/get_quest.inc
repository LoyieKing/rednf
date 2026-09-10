int UserQuest::get_quest(SIG_LOAD_QUEST* sig) const
{
    sig->m_field04 = 0;
    for (int i = 0; i <= 19; ++i)
    {
        sig->m_quest[i] = m_quest[i];
        sig->m_trigger[i] = m_trigger[i];
    }
    memcpy(sig->m_cleared, m_cleared.getClearedQuest(), 0x7530);
    sig->m_urgentQuest = m_user->getCurCharacQuestR()->m_authenData.m_urgentQuest;
    memcpy(sig->m_authen, m_authenData.m_authen, 0x28);
    return 1;
