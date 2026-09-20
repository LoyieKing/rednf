// ============================================================================
// CVillageMonsterMgr::OnKillVillageMonster(CUser*,bool) —— ORIG 0x86b4866
//   从队伍村战状态区取 zone（+0xc4c；0 → 0x15）与 vill/area（+0xc50/+0xc54），
//   Zone 命中区域转交 Area::OnKillVillageMonster(user, zone, flag)。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b4866）：去 view 具名指针
// （P0 每使用点完整 cast 表达式）+ 加 areaObj 具名（ORIG -0xc 槽），
// 消除 esi/ebx push 与帧差。p167 CLEAR。
int CVillageMonsterMgr::OnKillVillageMonster(CUser* user, bool flag)
{
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return 0x15;
    unsigned short zone =
        (unsigned short)party->m_battleField.m_villageMonsterId;
    if (zone == 0)
        return 0x15;
    Zone z((unsigned char)party->m_battleField.m_villageMonsterCount,
           (unsigned char)party->m_battleField.m_field130);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(z);
    if (it == m_areas.end())
        return 0x15;
    CVillageMonsterArea* areaObj = &it->second;
    return areaObj->OnKillVillageMonster(user, zone, flag);
}