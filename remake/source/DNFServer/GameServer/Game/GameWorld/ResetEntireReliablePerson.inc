void GameWorld::ResetEntireReliablePerson()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            if (user->getCurCharacR() != 0)
            {
                user->setCurCharacTradeGoldDaily(0);
                user->SetOverTradeGoldCount(0);
            }
        }
    }
}