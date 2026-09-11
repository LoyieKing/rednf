int BlueMarble::setLeaveUserBlueMarbleState()
{
    if (getWaitUserCount() == 0)
    {
        for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            m_users[i].resetBlueMarbleUserInfo();
        }
        changeState(BlueMarbleState::T_0);
        resetBlueMarble();
    }
    else
    {
        if (getState() == 1)
        {
            changeState(BlueMarbleState::T_0);
        }
    }
}