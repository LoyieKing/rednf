void CVillageMonster::SendVillageMonsterFightResult(CUser* user, bool flag)
{
    if (user == 0)
        return;
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return;

    Packet_VillageMonsterFightResult pkt;
    // ORIG ctor 0x86b4b84：PacketHeader(0x1772,0x2b) + m_flag=0 + m_keys[8] 清零。

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = party->get_user(i);
        if (member != 0)
        {
            pkt.m_uids[i] = member->GetUID();
            pkt.m_characNos[i] = member->get_charac_no(-1);
            if (GlobalData::s_villageMonsterMgr->OnEvent())
                if (flag)
                member->IncVillageAttackCount();
        }
    }
    pkt.m_flag = flag;

    CMonitorServerProxy* monitorProxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    monitorProxy->SendTcpPacket((char*)&pkt, 0x2b);

    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0xf9);
    pg.put_byte(flag);
    pg.finalize(true);
    party->send_to_party(pg);
