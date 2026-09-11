//   → total += m_killChecker1.m_useSkillMaterialTime(+0x4c+0x00)。
//   （ORIG 0830dea6 累加 -0xb4(%ebp)=MapInfo+0x4c=killChecker1 首字段；
//    2026-08-30 Fix21_C 由 m_field14 修正）
int CBattle_Field::GetTotalSkillMaterial()
{
    // ORIG 0x0830de27..0x0830de47 发射序：MapInfo::C1Ev(-0x100) → total=0(-0xc)
    // → iterator::C1Ev(-0x14) → map::begin；-O0 按语句序发射，故 info 必须声明在
    // total 之前（旧序 total→it→info 使三条初始化错位，本件唯一 DIFF 根因）。
    MapInfo info;
    int total = 0;
    // b28d-lane25：P167 差集实证 ORIG 走 iterator C1Ev + MapInfo C1Ev/D1/aSERKS_
    // ⇒ iterator 与 MapInfo 均在循环外"先声明"，循环内 info = it->second 赋值。
    std::map<int, MapInfo>::iterator it;
    for (it = m_mapInfo.begin(); it != m_mapInfo.end(); ++it)
    {
        info = it->second;
        if (info.m_dummyBossMap != 1 && info.m_visited != 0)
            total += info.m_killChecker1.m_useSkillMaterialTime;
    }
    return total;
}