void GameWorld::deleteSpecificItems(const std::vector<std::pair<int, int> >* list)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        ::deleteSpecificItems(user, list);
    }
