void GameWorld::InsertLoginUser(CUser* user)
{
    m_loginUserMap.insert(
        std::make_pair<unsigned int, CUser*&>(user->get_acc_id(), user));
