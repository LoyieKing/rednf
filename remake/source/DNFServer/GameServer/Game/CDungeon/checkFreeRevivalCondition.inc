bool CDungeon::checkFreeRevivalCondition(int diff) const
{
    // ORIG 0x08365298：it != end 且 !(value.m_4)（xor $1 取反测试）→ true
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    return it != m_map91c.end() && !it->second.m_4;
