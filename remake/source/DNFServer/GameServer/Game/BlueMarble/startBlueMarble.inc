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
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}