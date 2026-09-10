bool CParty::SendDungeonStatisticPartyCharac(bool flag, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b53aa：P235 具名成员视图替 helper
    // （删 EXTRA call）＋ P52 单语句链式（帧 0x78→0x74、&pkt 入 %ebx）。
    Packet_Dungeon_Statistic_Party_Charac pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_dungeonIndex = bf(this).get_dungeon_index();
    pkt.m_dungeonDiff = (unsigned char)bf(this).get_dungeon_diff();
    pkt.m_battleFlag354 = bd(this).m_flag28[0];   // party+0x354
    pkt.m_flag = flag;
    pkt.m_job = user->get_charac_job();
    pkt.m_growType = (unsigned char)user->getCurCharacGrowType();
    pkt.m_collect = (unsigned char)user->m_character.m_f20;
    pkt.m_playTime_f00 = user->m_character.m_f00;
    pkt.m_f04 = user->m_character.m_f04;
    pkt.m_f08 = user->m_character.m_f08;
    pkt.m_f0c = user->m_character.m_f0c;
    pkt.m_f10 = user->m_character.m_f10;
    pkt.m_f14 = user->m_character.m_f14;
    pkt.m_f18 = user->m_character.m_f18;
    pkt.m_f1c = user->m_character.m_f1c;
    pkt.m_f34 = user->m_character.m_f34;
    pkt.m_f30 = user->m_character.m_f30;
    pkt.m_level_f24 = (unsigned short)user->m_character.m_f24;
    if (flag)
        pkt.m_memberCount = (unsigned int)m_gameResultSet.m_field48;
    else
        pkt.m_memberCount = 0;

    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x47);
    return true;
