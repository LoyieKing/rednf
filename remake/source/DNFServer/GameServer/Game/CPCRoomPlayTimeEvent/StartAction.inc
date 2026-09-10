void CPCRoomPlayTimeEvent::StartAction()
{
    // ORIG 0x0811c754：取当前秒，遍历全服 m_UserInWorld，对 state>2 且受
    // 0xc 网吧优惠影响者置网吧起始时间并发送时长通知包。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user == 0)
            continue;
        if (user->get_state() <= 2)
            continue;
        if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
            continue;
        user->setPcRoomPlayStartTime(curSec);
        user->sendNotiPcRoomPlayTime();
    }
