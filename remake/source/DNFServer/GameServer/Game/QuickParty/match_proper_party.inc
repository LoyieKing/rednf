// ---- match_proper_party @0x08269f3c：为新 party 匹配同图快速队伍 ----
// [b28d-L1] 修法（P167/P220/P197④）：ORIG 0x08269f3c 调 ne×1 + list::size×1（无 eq/empty）⇒ 改正极性 `if (it != end) { 主体 }` + `size()==0`。差集仅余 N89（4.1 不内联 operator-> 发 ptEv×4）。残余槽位差 N82。
// [b28d-solo-QuickParty] verdict=AE（279=279，TU routed 口径；P167 CLEAR）：修法=
// ①去 `STQuickPartyPoolMap_Data& data = it->second` 引用缓存，每处重发 `it->second`
//   （P183；4.4 亦发 ptEv，N89 调用点差集消）；
// ②档位判空改正极性 `size() != 0` 包裹（P220 setne 形）；
// ③then/else 交换为 `if (need+memberCount == maxMembers)` 正极性（P222）；
// ④qp 两句式预清（ORIG 0x0826a128 `movl $0,-0x10`）；
// ⑤qpIndex 内联进 cMyTrace 实参（P227 全表达式临时链，%ebx 跨 ctor 保活）；
// ⑥`if (total != maxMembers) { trace+push } start_game(qp); return 0;` 块序（N93附）；
// ⑦traceMiss 改原位全表达式临时链。旧 N89/N82 判词作废。
ENUM_ERROR CQuickPartySystemManager::match_proper_party(
    const STQuickPartyPoolMap_Key& key, CParty* party, CUser* user)
{
    // [b28d-solo-QuickParty] 修法（P226）：ORIG 0x08269f4a 的 abnormal 物化在 %al
    // （mov $1/mov $0/test %al），无栈槽；具名 bool 使 4.4 发 `mov %al,-0x2d; cmpb $0,-0x2d`。
    if (({
            (party == 0) || (party->get_quick_party_index() != -1);
        }))
    {
        if (party == 0 && user != 0)
        {
            LogManager::logFormat(
                1, "QuickParty.cpp",
                __PRETTY_FUNCTION__,
                0x186, "QUICK_PARTY_LOG : PARTY IS NULL!! (%d)",
                reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacNo());
        }
        else
        {
            LogManager::logFormat(
                1, "QuickParty.cpp",
                __PRETTY_FUNCTION__,
                0x188, "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY INDEX (%d)!!",
                party->get_quick_party_index());   // [b28d-solo-QuickParty] P171 照抄：
                                                   // ORIG 0x08269f7x 无 party 判空直接调用
        }
        return static_cast<ENUM_ERROR>(0x12);
    }

    int err = check_match_proper_party(key, party);
    if (err != 0)
        return static_cast<ENUM_ERROR>(err);

    int memberCount = party->get_member_count();
    CDungeon* dungeon =
        G_CDataManager()->find_dungeon(static_cast<int>(key.m_dungeonIndex));
    if (dungeon == 0)
        return static_cast<ENUM_ERROR>(1);

    int maxMembers = 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        maxMembers = static_cast<int>(dungeon->get_limit_party_count());

    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    // [b28d-L1] 修法（P167/P220）：ORIG 34 calls 调 `_Rb_tree_iterator::ne`×1 +
    // `list::size`×1（无 eq/empty）⇒ 原码是正极性 `if (it != end) { 主体 }`；
    // 且档位判空写 `size() == 0` 而非 `empty()`（P197④）。原 eq/empty 形
    // P167 报 EXTRA eq/empty、MISSING ne/size。改写后调用点差集仅余 N89
    // （4.1 不内联 operator-> 发 ptEv×4，4.4 内联，结构性豁免）。
    if (it != m_poolMap.end())
    {
        // 从"还差 memberCount 人即可开团"的档位向下尝试归并。
        // [b28d-solo-QuickParty] 修法（P183/P220）：ORIG 0x0826a0xx 每处重发
        // `lea it; call ptEv; ...`（无 data 引用缓存），且档位判空为正极性
        // `size()!=0`（setne;test;je 直落 body）。
        for (int need = maxMembers - memberCount; need > 0; --need)
        {
            if (it->second.m_lists[need].size() != 0)
            {
                if (need + memberCount == maxMembers)
                {
                    // 正好补齐：弹出该档 quick party 并立即尝试开局。
                    // ORIG 0x0826a128 预清 qp（movl $0,-0x10）后赋值。
                    CQuickParty* qp = 0;
                    qp = find_quick_party_in_pool(it->second.m_lists[need]);
                    if (qp == 0)
                        continue;

                    qp->set_party_index(party->GetPartyIndex());
                    int total = qp->get_quick_party_count();
                    // [b28d-solo-QuickParty] 修法（P222/N93附）：ORIG 0x0826a18c
                    // `cmp -0x18; je <后置 start_game 块>`（相等时跳过 trace 块）
                    // ⇒ 源码须负极性包 trace 块、start_game 直落后置。
                    if (total != maxMembers)
                    {
                        // [b28d-solo-QuickParty] 修法（P0/P227）：ORIG 0x0826a1a0
                        // 的 get_quick_party_index 结果入 %ebx 跨 cMyTrace ctor
                        // 保活（无具名槽）⇒ 实参内联，勿声明 qpIndex 局部。
                        cMyTrace(__PRETTY_FUNCTION__, 0x1b4, 0)(
                            "QUICK_PARTY_LOG : REPUSH QUICK PARTY IN POOL!! QUICK_PARTY_INDEX(%d), QUICK_PARTY_COUNT(%d)",
                            static_cast<unsigned int>(qp->get_quick_party_index()),
                            total);
                        if (!push_quick_party_in_pool(key, it->second, party,
                                                      total, qp, true))
                        {
                            LogManager::logFormat(
                                1, "QuickParty.cpp",
                                __PRETTY_FUNCTION__,
                                0x1bc, "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY!! (%d)", total);
                            return static_cast<ENUM_ERROR>(1);
                        }
                        return static_cast<ENUM_ERROR>(0);
                    }
                    start_game(qp);
                    return static_cast<ENUM_ERROR>(0);
                }
                else
                {
                    // 尚不足开局人数：并入现有等待队列（注册新组合）。
                    if (!regist_quick_party(key, it->second, need, party,
                                            memberCount))
                        return static_cast<ENUM_ERROR>(1);
                    return static_cast<ENUM_ERROR>(0);
                }
            }
        }

        // 无可归并组合：以空队首领新池对象挂起等待。
        if (!regist_quick_party(key, it->second, 0, party, memberCount))
            return static_cast<ENUM_ERROR>(1);
        return static_cast<ENUM_ERROR>(0);
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x1d6, 5)(
        "QUICK_PARTY_LOG : NO EXIST KEY DATA!!");
    return static_cast<ENUM_ERROR>(1);
}