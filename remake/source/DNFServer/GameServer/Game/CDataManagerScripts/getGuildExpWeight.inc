// ORIG D1 0x837c3d4 (W)：析构成员 map<int,rewardInfoList>。
// ---- GuildParameterScript::getGuildExpWeight（ORIG T 0x089795de）----
// m_map44.find(level) 命中返回 second，否则 0（CUser::FatigueUp 公会经验权重，
// 与 0x08979864 getExpWeight 的 m_vecE4 扫表路径不同；ORIG 返回 int）。
int GuildParameterScript::getGuildExpWeight(int level)
{
    std::map<int, int>::iterator it = m_map44.find(level);
    if (it != m_map44.end()) return it->second;
    return 0;
}