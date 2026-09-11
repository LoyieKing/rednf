// init_quick_party_pool @0x08269c0c：
// 该 dungeon 已登记于快速派对奖励副本表 → 直接返回；
// 非绝望之塔副本时，按 4 个难度档位向池 map 插入空数据
// （血/任务 NPC 副本仅插入档位 0）。
// [b28d-L1] 修法（P167/P96）：ORIG 0x08269c0c 只调 `__gnu_cxx::ne<const int*>`（两侧 const_iterator）；非 const 引用使 4.4 多发 begin/end+转换构造+混合 ne(PKi,Pi)（EXTRA×4/MISSING×1）。改 const 引用后迭代器差集清零；残余帧差 0x90 vs 0xa0（N82/P232）。
// [b28d-solo-QuickParty] verdict=AE（163=163，TU routed 口径；P167 CLEAR）：修法=
// P214① 正极性早返 `if (dungeon->isTowerOfDespairDungeon()) return;`（复现 ORIG
// 0x08269cc6 `test %al,%al; jne <nop 岛>`，消 `xor $0x1` 反形）+ 三条件正向 if 包裹
// （ORIG 0x08269d3a or 链物化后 `test %al,%al; je <插入块>` 直落）+ blood 条件
// `(signed char)` 化复现 `test %al,%al; jg`。旧「N96/P292 xor 不可翻」判词作废。
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
}