bool SecretShopScript::GetItemByDungeonLev(CMTRand* rand,
                                           std::vector<secretshop::SALE_INFO>& out,
                                           int dungeonIdx, int level, bool event)
{
    std::map<int, stNpcSales>::iterator npcIt = m_dungeonNpcMap.find(dungeonIdx);
    if (npcIt == m_dungeonNpcMap.end())
        return 0;
    int levelIdx = GetLevelIdx(level);
    std::map<int, stDungeonSales>::iterator dunIt =
        npcIt->second.m_eventSales.find(levelIdx);
    if (dunIt == npcIt->second.m_eventSales.end())
        return 0;
    // [b28-L14] P0 清绑（同上）。
    if (!GetItems(rand, out, dunIt->second))
        return false;
    if (event)
        GetItemByEvent(rand, out);
    return true;
