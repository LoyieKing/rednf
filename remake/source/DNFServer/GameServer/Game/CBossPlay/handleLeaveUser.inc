bool CBossPlay::handleLeaveUser(CUser* user)
{
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        // P133：ORIG 两守卫独立 jne（checkValidUser 直跳 + get_user==user
        // sete 岛），合并 && 生 bool 展开形。拆两条 + 直比（同型件
        // CDeathTower::handleLeaveUser 清绑后 AE 先例）。
        if (m_party->checkValidUser(i))
        {
        if (m_party->get_user(i) == user)
        {
            if (m_memberField[i] == 0 && user->get_state() == 0xc)
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (ENUM_HACKTYPE)0x13a, 1, 0, 0);
            }
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            setMemberAlive(i, false);
        }
        }
    }

    G_GameWorld()->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);
    return 1;
