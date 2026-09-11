// ---- check_quick_party_min_count @0x0826ab9e：开局最低人数校验 ----
bool CQuickPartySystemManager::check_quick_party_min_count(
    const STQuickPartyPoolMap_Key& key, int count)
{
    int dungeonIdx = static_cast<int>(static_cast<short>(key.m_dungeonIndex));
    int diff =
        static_cast<int>(static_cast<signed char>(key.m_dungeonDiff));   // ORIG movsbl 32 位存储
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return false;

    if (dungeon->get_blood_dungeon_type() == 2)
    {
        if (static_cast<int>(
                static_cast<signed char>(dungeon->get_limit_party_count())) !=
            count)
            return false;
    }
    else if (static_cast<signed char>(dungeon->get_dimension_possible()) > 0)
    {
        // 次元可入副本：允许维度最低人数 + 1。
        if (count <
            static_cast<int>(
                static_cast<signed char>(dungeon->get_dimension_min_partymem())) +
                1)
            return false;
    }
    else
    {
        // 次元属性关闭：查 QuickParty.etc 的 minCount 表
        //（表键 = dungeon+0x89c 入场限制组合位非零 ? 1 : 0）。
        int requireMin = -1;
        int tableRow = -1;
        if (dungeon->m_89c != 0)
            tableRow = 1;
        else
            tableRow = 0;
        if (G_CGameManager()->GetQuickPartyRewardManager() != 0)
            requireMin =
                G_CGameManager()->GetQuickPartyRewardManager()
                    ->getRequireQuickPartyMinCount(tableRow, diff);
        if (requireMin != -1 && requireMin > count)
            return false;
        if (count < 2)
            return false;
    }
    return true;
}