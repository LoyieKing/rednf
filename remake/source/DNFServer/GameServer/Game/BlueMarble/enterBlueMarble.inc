int BlueMarble::enterBlueMarble(CUser* user)
{
    int result = 0;
    if (user == 0)
    {
        return 0x17;
    }
    if (!checkJoin())
    {
        return 0x17;
    }
    result = checkUserConditionEnterBlueMarble(user);
    if (result > 0)
    {
        return result;
    }
    result = join(user);
    if (result > 0)
    {
        return result;
    }
    if (getWaitUserCount() == m_pScript->m_info.m_playerMax)
    {
        changeState((BlueMarbleState::T)1);
    }
    return 0;
}