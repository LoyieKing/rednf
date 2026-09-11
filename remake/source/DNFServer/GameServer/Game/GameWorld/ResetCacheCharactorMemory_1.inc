bool GameWorld::ResetCacheCharactorMemory(unsigned int accId)
{
    std::map<unsigned int, CUser*>::iterator it = m_loginUserMap.find(accId);
    if (it != m_loginUserMap.end())
    {
        CUser* user = it->second;
        user->SetValidLastLoginCharac(false);
        user->SetLastLoginCharacNo(0);
        user->SetLastLoginChannelNo(0);
        return true;
    }
    return false;
}