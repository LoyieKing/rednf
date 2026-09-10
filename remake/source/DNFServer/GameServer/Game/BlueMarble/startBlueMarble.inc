int BlueMarble::startBlueMarble()
{
    if (checkStartBlueMarble())
    {
        resetBlueMarble();
        changeState((BlueMarbleState::T)2);
        setBlueMarbleMap();
        setMaxBoardSize();
        for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            if (m_users[i].getUser() == 0)
            {
                continue;
            }
            m_users[i].setUserState((BlueMarbleUserState::T)1);
            m_users[i].getUser()->increaseBlueMarbleEnterCount();
            m_users[i].getUser()->sendBlueMarbleEnterCount();
        }
        sendStartBlueMarble();
    }
