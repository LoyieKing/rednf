bool UserQuest::set_quest(SIG_LOAD_QUEST const* sig)   // [b28c-L9] SIGFIX:UserQuest::set_quest (ORIG 串 bool)
{
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = sig->m_quest[i];
        m_trigger[i] = sig->m_trigger[i];
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest == 0 && m_quest[i] != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x42, 0)
                ("VERIFY_QUEST : ABNOMAL QUEST INDEX (%d) : CHARAC_NO (%u)",
                 m_quest[i], sig->m_characNo);
            m_quest[i] = 0;
            m_trigger[i] = 0;
        }
    }
    m_cleared.setClearedQuest(
        (const WongWork::STClearedQuest*)&sig->m_cleared[0]);
    m_authenData.m_urgentQuest = sig->m_urgentQuest;
    memcpy(m_authenData.m_authen, sig->m_authen, 0x28);
    return 1;
}