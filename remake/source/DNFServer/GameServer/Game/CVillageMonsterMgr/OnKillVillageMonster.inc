int CVillageMonsterMgr::OnKillVillageMonster(CUser* user, bool flag)
{
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return 0x15;
    unsigned short zone =
        ((CPartyVillageStateView*)party)->m_villageMonsterZone;
    if (zone == 0)
        return 0x15;
    Zone z((unsigned char)((CPartyVillageStateView*)party)->m_villageVill,
           (unsigned char)((CPartyVillageStateView*)party)->m_villageArea);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(z);
    if (it == m_areas.end())
        return 0x15;
    CVillageMonsterArea* areaObj = &it->second;
    return areaObj->OnKillVillageMonster(user, zone, flag);
