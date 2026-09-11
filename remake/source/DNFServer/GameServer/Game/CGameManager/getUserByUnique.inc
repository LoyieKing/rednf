CUser* CGameManager::getUserByUnique(short uniqueId)
{
    std::map<unsigned int, CUser*>::iterator it;
    it = std::find_if(m_userByAccId.begin(), m_userByAccId.end(),
                      FindUserByUnique(uniqueId));
    if (it != m_userByAccId.end())
        return it->second;
    return 0;
}