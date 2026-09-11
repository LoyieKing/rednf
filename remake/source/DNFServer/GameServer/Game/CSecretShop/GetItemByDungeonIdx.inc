// ============================================================================
// SecretShopScript::GetItemByDungeonIdx / GetItemByDungeonLev（ORIG 0x08a82a0e /
// 0x08a82b34）。依赖链：GetItems -> CopyItem/IsExistItem/GetDomainRate，GetLevelIdx，
// GetItemByEvent -> GetItems。全部按 class_func_reports/SecretShopScript 忠实转写。
// ============================================================================
bool SecretShopScript::GetItemByDungeonIdx(CMTRand* rand,
                                           std::vector<secretshop::SALE_INFO>& out,
                                           int dungeonIdx, int level, bool event)
{
    std::map<int, stNpcSales>::iterator npcIt = m_dungeonNpcMap.find(dungeonIdx);
    if (npcIt == m_dungeonNpcMap.end())
        return 0;
    std::map<int, stDungeonSales>::iterator dunIt =
        npcIt->second.m_dungeonSales.find(level);
    if (dunIt == npcIt->second.m_dungeonSales.end())
        return 0;
    // [b28-L14] P0 清绑：原 `register unsigned char fv asm("eax")` 禁用。
    // ORIG `call GetItems; xor $0x1,%eax; test %al,%al; je <主>` ⇒ bool callee
    // 裸否定早返（P168a①）。
    if (!GetItems(rand, out, dunIt->second))
        return false;
    if (event)
        GetItemByEvent(rand, out);
    return true;
}