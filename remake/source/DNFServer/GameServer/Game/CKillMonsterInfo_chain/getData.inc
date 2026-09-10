void CKillMonsterInfo::getData(char* pData) const
{
    char* p = pData;
    ConvertMapToStrcut(m_bossKills, (stMonsterInfo_t*)p, 0x5dc);
    ConvertMapToStrcut(m_namedKills, (stMonsterInfo_t*)(p + 0x1d4c), 6000);
    ConvertMapToStrcut(m_apcBossKills, (stMonsterInfo_t*)(p + 0x927c), 0x5dc);
