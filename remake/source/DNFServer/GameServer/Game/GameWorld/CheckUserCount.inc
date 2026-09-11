int GameWorld::CheckUserCount(int add)
{
    int size = m_UserInWorld.size();
    int maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    // ORIG mov 0xc(=add),%eax 先载：左操作数=add 决定 lea 寄存器分配
    if (size + add > maxUserNum)
    {
        return false;
    }
    return true;
}