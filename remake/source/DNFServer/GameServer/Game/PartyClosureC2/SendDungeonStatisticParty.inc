bool CParty::SendDungeonStatisticParty(bool flag, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b5158：P52 单语句链式复现
    // `lea &pkt→%ebx` 跨 GetServerProxy 保活（具名 proxy 局部多槽 + 帧 0x78→0x64）。
    // 报告：docs/class_func_reports/CParty/SendDungeonStatisticParty.md（T 0x085b5158）：
    // 0x40 字节统计包（header id 0xc35），字段偏移逐条对应报告汇编。
    Packet_Dungeon_Statistic_Party pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_dungeonIndex = bf(this).get_dungeon_index();
    pkt.m_dungeonDiff = (unsigned char)bf(this).get_dungeon_diff();
    pkt.m_battleFlag355 = bd(this).m_flag28[1];   // party+0x355
    pkt.m_battleFlag356 = bd(this).m_flag28[2];   // party+0x356
    pkt.m_battleFlag357 = bd(this).m_flag28[3];   // party+0x357
    pkt.m_flag = flag;
    pkt.m_collect = (unsigned char)user->m_character.m_f20;   // +0x8e054
    pkt.m_playTime_f00 = user->m_character.m_f00;             // +0x8e034
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

    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x40);
    return true;
