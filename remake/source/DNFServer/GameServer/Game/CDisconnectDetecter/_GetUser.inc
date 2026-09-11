CUser* CDisconnectDetecter::_GetUser(int uid, int uniqueId)
{
    CUser* user = G_CGameManager()->getUser(uid, 2);
    if (user != 0)
    {
        if ((unsigned short)user->get_unique_id() != uniqueId)
        {
            return 0;
        }
    }
    return user;
}