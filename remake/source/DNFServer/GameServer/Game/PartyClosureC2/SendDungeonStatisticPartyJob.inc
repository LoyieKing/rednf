bool CParty::SendDungeonStatisticPartyJob(bool flag, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b52a6：
    //   P0/P235 具名成员视图替 helper（m_resultView.m_resultField48@+0x183c
    //   复现 ORIG `movzwl 0x183c(%eax)`，删 EXTRA call）；
    //   P52 单语句链式 `GetServerProxy(...)->SendPacket(&pkt,0x1f)` 复现
    //   `lea &pkt→%ebx` 跨调用保活（具名 proxy 局部多 -0xc 槽 + 帧 0x58→0x44）。
    Packet_Dungeon_Statistic_Party_Job pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_dungeonIndex = bf(this).get_dungeon_index();
    pkt.m_dungeonDiff = (unsigned char)bf(this).get_dungeon_diff();
    pkt.m_battleFlag355 = bd(this).m_flag28[1];
    pkt.m_battleFlag356 = bd(this).m_flag28[2];
    pkt.m_battleFlag357 = bd(this).m_flag28[3];
    pkt.m_flag = flag;
    pkt.m_job = user->get_charac_job();
    pkt.m_growType = (unsigned char)user->getCurCharacGrowType();
    pkt.m_collect = (unsigned char)user->m_character.m_f20;
    if (flag)                                                   // party+0x183c
        pkt.m_memberCount = (unsigned int)m_gameResultSet.m_field48;
    else
        pkt.m_memberCount = 0;

    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x1f);
    return true;
