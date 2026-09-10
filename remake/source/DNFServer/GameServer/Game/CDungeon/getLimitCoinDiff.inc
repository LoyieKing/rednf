bool CDungeon::getLimitCoinDiff(int diff, int& out) const
{
    // ORIG 0x08365316：size==0 → 0；命中取 value.m_0 写 out 返回 1；未命中返回 0
    if (m_map91c.size() == 0)
        return 0;
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    if (it != m_map91c.end())
    {
        out = it->second.m_0;
        return 1;
    }
    return 0;
