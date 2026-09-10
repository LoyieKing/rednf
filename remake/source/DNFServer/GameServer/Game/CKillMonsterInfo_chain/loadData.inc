bool CKillMonsterInfo::loadData(CUser* pUser, char* pData)
{
    _reset();
    char* p = pData;
    ConvertStructToMap((stMonsterInfo_t*)p, 0x5dc, m_bossKills, false);
    ConvertStructToMap((stMonsterInfo_t*)(p + 0x1d4c), 6000, m_namedKills,
                       false);
    ConvertStructToMap((stMonsterInfo_t*)(p + 0x927c), 0x5dc, m_apcBossKills,
                       true);
    return true;
