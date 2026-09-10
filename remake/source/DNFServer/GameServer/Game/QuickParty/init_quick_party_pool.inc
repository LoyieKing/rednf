void CQuickPartySystemManager::init_quick_party_pool(const CDungeon* dungeon)
{
    if (dungeon == 0)
        return;

    // [b28d-solo-QuickParty] 修法（P0/P24）：ORIG 0x08269c31/0x08269c58 两次调用
    // get_dungeon_idx_list()（begin/end 各一次），无 idxList 引用槽；引用局部使 4.4
    // 多发 `mov %eax,-0x14` 槽往返并把帧撑到 0xa0（ORIG 0x90）。
    std::vector<int>::const_iterator it =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()
                ->get_dungeon_idx_list()).begin();
    std::vector<int>::const_iterator end =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()
                ->get_dungeon_idx_list()).end();
    for (; it != end; ++it) {
        if (dungeon->get_index() == *it)
            return;                                     // 已是 quick party 奖励副本
    }

    if (dungeon->isTowerOfDespairDungeon())
        return;

    {
        STQuickPartyPoolMap_Key key;
        STQuickPartyPoolMap_Data data;

        key.m_dungeonIndex = (short)dungeon->get_index();
        for (int i = 0; i < 4; ++i) {
            // [b28d-solo-QuickParty] 修法（P214① 正向包裹 + P223 变体）：
            // ORIG 0x08269cc6 `test %al,%al; jne <nop 岛>`（正极性早返，非 xor 形），
            // 0x08269d3a 的 or 链物化后 `test %al,%al; je <插入块>` 直落（非 xor 反形）。
            if ((dungeon->getDungeonKind() == 1) ||
                (static_cast<signed char>(dungeon->get_blood_dungeon_type()) > 0) ||
                (dungeon->getQuestNpcDungeon() != 0)) {
                if (i != 0)
                    goto next_diff;
            }
            key.m_dungeonDiff = (signed char)i;
            m_poolMap.insert(std::make_pair(key, data));
        next_diff: ;
        }
    }
