int CVillageMonsterMgr::OnFightVillageMonster(CUser* user, unsigned short zone)
{
    Zone findZone(user->getCurCharacVill(),
                  (unsigned char)user->get_area(false));
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(findZone);
    if (it == m_areas.end())
        return 0x15;
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return 0x15;
    CVillageMonsterArea* areaObj = &it->second;
    int out1 = 0;   // → monster.m_field20 → party+0xcb8
    int out2 = 0;   // → monster.m_field24 → party+0xc58
    int ret = areaObj->OnFightVillageMonster(user, zone, out1, out2);
    if (ret == 0)
    {
        ((CPartyVillageStateView*)party)->m_villageMonsterZone = zone;
        ((CPartyVillageStateView*)party)->m_villageVill =
            (int)(signed char)user->getCurCharacVill();
        ((CPartyVillageStateView*)party)->m_villageArea = user->get_area(false);
        ((CPartyVillageStateView*)party)->m_villageMonsterField24 = out2;
        ((CPartyVillageStateView*)party)->m_villageMonsterField20 = out1;
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0xf3);
        packet.finalize(true);
        party->send_to_party(packet);
    }
    return ret;
