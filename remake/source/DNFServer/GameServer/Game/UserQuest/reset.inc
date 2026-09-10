void UserQuest::reset()
{
    m_user = 0;
    m_cleared.clear();
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = 0;
        m_trigger[i] = 0;
    }
    m_authenData.m_urgentQuest = -1;
    memset(m_authenData.m_authen, 0, 0x28);
