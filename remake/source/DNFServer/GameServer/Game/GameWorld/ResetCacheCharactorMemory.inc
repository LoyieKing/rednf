void GameWorld::ResetCacheCharactorMemory()
{
    for (std::map<unsigned int, CUser*>::iterator it = m_loginUserMap.begin();
         it != m_loginUserMap.end(); it++)
    {
        CUser* user = it->second;
        user->SetValidLastLoginCharac(false);
        user->SetLastLoginCharacNo(0);
        user->SetLastLoginChannelNo(0);
    }
