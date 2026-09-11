void GameWorld::EraseLoginUser(CUser* user)
{
    std::map<unsigned int, CUser*>::iterator it =
        m_loginUserMap.find(user->get_acc_id());
    if (it != m_loginUserMap.end())
    {
        m_loginUserMap.erase(it);
    }
}