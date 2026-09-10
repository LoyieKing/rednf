void CBattle_Field::add_monster_list_bossroom_condition(std::list<_mapMonster>& mobList,
                                                        int x, int y)
{
    // [b28d-Lane27] ORIG 0x083013f5/0x08301406 两条独立 `je`（各带岛）+ 0x0830141e
    // `jne <epilogue>`（直跳）⇒ 拆三条独立守卫。
    // [F3b-C05③ 2026-09-10 翻案 ⇒ AE（173=173 逐条全等）] 旧判「残余 1 对
    // `jmp epilogue;nop` 属 epilogue-threading 不可控」**作废**（round5-C05 当时
    // 只试了 `||` 合并 / goto 尾标签两形，均零收益）：第三条守卫（ORIG 0x0830141e
    // `jne <epilogue>` **直达**）改**正向包裹** `if (m_pDungeon->m_bossRoom90c.m_0
    // == 1) { … }` 后 4.4 把 fail 路径 jump-thread 直达共享尾出口 ⇒ 尾岛 3→2、
    // our 175→173（= ORIG 173）。前两条守卫（各带岛）保持平铺 `return;` 不动。
    if (m_pDungeon == NULL)
        return;
    if (m_pMap == NULL)
        return;
    if (m_pDungeon->m_bossRoom90c.m_0 == 1)
    {
        // ORIG：外层 const_iterator（begin/end 各转换构造一次，4 槽）+ 缓存 end；内层
        // const_iterator 无转换（begin() 已返 const）+ 缓存 end（P13）。
        std::vector<stConditionEnterBossRoomStatus_t>::const_iterator cond =
            m_bossRoomConditions.begin();
        std::vector<stConditionEnterBossRoomStatus_t>::const_iterator condEnd =
            m_bossRoomConditions.end();
        for (; cond != condEnd; ++cond)                                   // +0x170
        {
            std::vector<std::pair<int, int> >::const_iterator pos =
                cond->m_pos.begin();
            std::vector<std::pair<int, int> >::const_iterator posEnd =
                cond->m_pos.end();
            for (; pos != posEnd; ++pos)
            {
                if (pos->first == x && pos->second == y)
                {
                    const std::list<_mapMonster>& conditions =
                        m_pMap->get_condition_monster_list();
                    for (std::list<_mapMonster>::const_iterator mobIt =
                             conditions.begin();
                         mobIt != conditions.end(); ++mobIt)
                    {
                        if (mobIt->m_mobId == cond->m_monsterIdx)
                            mobList.push_back(*mobIt);
                    }
                }
            }
        }
    }
