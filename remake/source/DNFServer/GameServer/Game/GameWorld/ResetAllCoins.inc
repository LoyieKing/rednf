void GameWorld::ResetAllCoins()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        user->RecoverCoin(1);
        if (user->GetOpencoin() == 0)
        {
            user->SendOpenflag();
        }
    }
