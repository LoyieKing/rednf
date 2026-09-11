bool CGameManager::isConnectUserByAccId(unsigned int accId)
{
    for (int i = 0; i < 600; ++i)
    {
        CUser* user = m_userPool.Get(i);
        if (user->get_acc_id() == accId)
            return 1;
    }
    return 0;
}