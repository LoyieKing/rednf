// ORIG 0x083013e2：boss 房条件副本（m_pDungeon->m_bossRoom90c.m_0==1）时，把
// 与条件坐标匹配的怪物从条件怪表并入 mobList。
// [b28d-solo-BattleMonsterDepsC2] P214①/P13（已修 168→175，正文逐条全等）：ORIG 无 matched
// 具名 bool/无 break——匹配即执行内层 list 拷贝+push_back 后继续遍历（`mov $1/$0` 短路物化）；
// 外层 const_iterator 转换构造 + begin/end 各一次（4 槽），内层 const_iterator 缓存 end；
// conditions 为 const 引用绑定临时（ORIG local_10 指针槽，无 4.4 值拷贝）。
// [SweepBattleMonster 2026-09-09] 残差 N121：尾部 4.4 三层循环出口各发 nop+jmp 岛（ORIG 2 岛，
// 第三层直落 lea esp）⇒ clean 171 vs 172（+1 条）。
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
}