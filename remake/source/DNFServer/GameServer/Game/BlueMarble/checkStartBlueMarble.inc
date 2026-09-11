bool BlueMarble::checkStartBlueMarble()
{
    if (getState() != 1)
    {
        return 0;
    }
    if (getWaitUserCount() < m_pScript->m_info.m_playerMax)
    {
        changeState(BlueMarbleState::T_0);
        return 0;
    }
    return 1;
}