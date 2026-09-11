CUser* GameWorld::find_from_world(unsigned short uniqueId)
{
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UserInWorld.find(uniqueId);
    if (it == m_UserInWorld.end())
    {
        return NULL;
    }
    return it->second;
}