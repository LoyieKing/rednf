void SeriaRoomAniDecoEvent::StartAction()               // 0x0827b6a6
{
    // 全服 state>2 用户广播 SendEventInfo。
    // [b28d-28] 同 GiveGrowCreatureEvent::StartAction（P183 去缓存 + P28 后置）。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            SendEventInfo(*user);
        }
    }
