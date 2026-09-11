bool BlueMarble::checkJoin()
{
    if (getState() != 0)
    {
        return 0;
    }
    if (getWaitUserCount() >= m_pScript->m_info.m_playerMax)
    {
        return 0;
    }
    return 1;
}