// ============================================================================
// CDungeonJobNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08484bac
// find(stat->m_skillKey)；命中则 it->second += stat->m_charac[0x40]（Skill 聚合）。
// ============================================================================
bool CDungeonJobNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    // [b28d-39] ORIG 0x08484bac：find sret -0x14 / end sret -0x10 / s -0xc；
    // 聚合源地址 `add $0x64,%eax`（单条）⇒ 必须写 &m_charac[0x40]（数组下标折叠），
    // `m_charac + 0x40` 在 4.4 生 add $0x24 + add $0x40 两条。
    std::map<int, Skill>::iterator it = m_skillMap.find(stat->m_skillKey);
    if (it == m_skillMap.end())
        return false;
    Skill& s = it->second;
    s += *reinterpret_cast<const Skill*>(&stat->m_charac[0x40]);
    return true;
}