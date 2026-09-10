void SeriaRoomAniDecoEvent::dailyresetData()            // 0x0827b5f0
{
    // 全服 state<=2 用户跳过；state>2 用户尝试发放当日摆件奖励（ORIG 顺序）。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
            continue;
        SendAniDecoItem(*user);
    }
