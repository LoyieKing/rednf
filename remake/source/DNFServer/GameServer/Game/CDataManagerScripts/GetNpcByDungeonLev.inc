bool SecretShopScript::GetNpcByDungeonLev(int& out, int a, int b)
{
    // ORIG 0x8a8293c：GetLevelIdx(b) → +0x18（m_levelNpcMap）equal_range →
    // GetNpcIdx（返回其 char 结果）。
    int levelIdx = GetLevelIdx(b);
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator> range =
        m_levelNpcMap.equal_range(levelIdx);
    return GetNpcIdx(out, a, range);
}