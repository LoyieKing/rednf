void CPCRoomPlayTimeEvent::EndEvent()
{
    // ORIG 0x0811c630：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x1f, 0)，
    // 再遍历全服 m_UserInWorld，对 state>2 且受 0xc 网吧优惠影响者清空网吧时长数据。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("PC Room PlayTime Event End!");
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
        user->setPcRoomPlayTimeData(0, 0, 0);
    }
}