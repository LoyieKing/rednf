// ---- loadData —— ORIG T 0x0854cddc ----------------------------------------
// 开头虚调 vptr[+0x1c] = _reset()（ORIG vtable 槽 7 实证）；三块顺序加载，
// APC 槽 bAPC=true（第三块）；返回 true。
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
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
}