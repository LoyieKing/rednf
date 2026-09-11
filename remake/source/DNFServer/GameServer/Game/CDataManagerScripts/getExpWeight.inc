// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----
// 自高向低扫 m_vecE4（vector<pair<int,double>>，等级阈值→经验权重），
// 首个 first<=guildLevel 的 second；全不命中取 [0].second（ORIG 无判空，照抄）。
double GuildParameterScript::getExpWeight(int guildLevel)
{
    for (int i = (int)m_vecE4.size() - 1; i >= 0; --i)
    {
        if (m_vecE4[i].first <= guildLevel)
            return m_vecE4[i].second;
    }
    return m_vecE4[0].second;
}