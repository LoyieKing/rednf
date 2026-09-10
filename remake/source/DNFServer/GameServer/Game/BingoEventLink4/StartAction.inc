void BingoEvent::StartAction()
{
    // [b28d-D9] ORIG 0x080c9bb8 用 BOOST_FOREACH 展开（contain/begin/end/deref/next/
    // done 全序列），非手写迭代器循环；容器为 G_CGameManager()+0x22c map。
    typedef std::map<unsigned int, CUser*>::value_type UserPair;
    BOOST_FOREACH(UserPair& pr, G_CGameManager()->m_userByAccId)
    {
        CUser* user = pr.second;
        if (user != 0)
        {
            user->sendBingoData();
        }
    }
