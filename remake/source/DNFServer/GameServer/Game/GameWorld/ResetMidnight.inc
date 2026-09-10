void GameWorld::ResetMidnight()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        user->ResetDailyCharacExpandDataMidnight();
    }
