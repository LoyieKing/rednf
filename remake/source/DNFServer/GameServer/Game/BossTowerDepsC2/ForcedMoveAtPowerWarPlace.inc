void CAssaultPlace::ForcedMoveAtPowerWarPlace(CUser* user)
{
    if (({ GlobalData::s_power_manager->IsPowerWarEventOn() &&
           user->getCurCharacVill() == 7; }))
    {
        CForcedMoveCharacter request;   // ORIG 栈对象，无构造调用
        request.m_user = user;
        if (user->getPowerSide() == 1)
        {
            request.m_destVill = 7;
            request.m_destArea = 0;
            request.m_x = 0x255;
            request.m_y = 300;
        }
        else if (user->getPowerSide() == 2)
        {
            request.m_destVill = 7;
            request.m_destArea = 4;
            request.m_x = 0x139;
            request.m_y = 0x13a;
        }
        request.ForcedMove();
    }
