// ---- getData —— ORIG T 0x0854ce90（const，void）---------------------------
// 三 map 顺序落盘：+0x0 / +0x1d4c / +0x927c（1500 / 6000 / 1500 项 × 5 字节）。
void CKillMonsterInfo::getData(char* pData) const
{
    char* p = pData;
    ConvertMapToStrcut(m_bossKills, (stMonsterInfo_t*)p, 0x5dc);
    ConvertMapToStrcut(m_namedKills, (stMonsterInfo_t*)(p + 0x1d4c), 6000);
    ConvertMapToStrcut(m_apcBossKills, (stMonsterInfo_t*)(p + 0x927c), 0x5dc);
}