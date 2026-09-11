// [b28d-solo-CListTables 修复] ORIG 0x0835556c：131/131（gap=0）+ p167 CLEAR(16/16)。
// 已修：range 提到函数头（C1Ev 在循环外）、三守卫独立 goto + 尾标签串（P223 多岛，
// ORIG +0x136/+0x139/+0x13c 三 nop 岛复现）、flag 守卫改嵌套 `if (!flag) {...}`
// 复现 ORIG `movzbl -0x3c; xor $1; test %al; je`（旧 `if (flag) goto DO_PUSH` 被
// 4.4 折成 `cmpb $0; jne`）。残差仅 it/range/param 槽位差（N82 族）⇒ 封顶。
// [round1-L29 2026-09-10 修复] 判据⑩：`int npcIdx;` 前置声明（无初值→浅槽 -0x24）+ 循环内改
//   赋值 `npcIdx = *it;`（保留 it→range→npcIdx 调用序）⇒ AE 128/128 ndiff=0（此前 NEAR）。
//   注：把 cit 也提到头部（全头部声明）反劣 131 vs 128，已回退。
void QuestList::allowable_questlist_as_npc(
    std::list<int>& out, const std::multimap<int, int>& npcList,
    stSelectQuestParam& param, const WongWork::CQuestClear& clear, bool flag)
{
    // [b28d-solo-CListTables] ORIG 0x0835556c：it 先 C1Ev 后 begin() 赋值（P50）
    // + 尾后置 it++（ppEi）；range 先 C1Ev（-0x2c，函数头）再
    // `range = equal_range(...)`（op=(&&) 赋值，同 GetDailyQuestList）。
    std::list<int>::iterator it;
    int npcIdx;
    Quest* quest = 0;
    std::pair<std::multimap<int, int>::const_iterator,
              std::multimap<int, int>::const_iterator> range;
    it = m_listE0.begin();
    while (it != m_listE0.end())
    {
        npcIdx = *it;
        range = npcList.equal_range(npcIdx);
        for (std::multimap<int, int>::const_iterator cit = range.first;
             cit != range.second; ++cit)
        {
            param.m_questIdx = cit->second;
            quest = find_quest(param.m_questIdx);
            // [b28d-solo-CListTables] ORIG 0x083556a2/0x083556a5/0x083556a8：三失败
            // 出口各带 nop 中继岛（+0x136/+0x139/+0x13c）⇒ P223 多岛形：独立否定
            // goto + 尾标签串（标签序 = 守卫序）。
            if (quest == 0)
                goto SKIP_NULL_QUEST;
            if (!Check_Quest(quest, param, clear))
                goto SKIP_CHECKQ_FAIL;
            // [b28d-solo-CListTables P168a/P194] ORIG 0x083556.. `movzbl -0x3c;
            // xor $1; test %al; je`（bool 形参取反物化）⇒ 写嵌套 `if (!flag)`，
            // 旧 `if (flag) goto DO_PUSH;` 被 4.4 折成 `cmpb $0; jne`（2 条）。
            if (!flag)
            {
                if (!check_clear(param.m_questIdx, clear))
                    goto SKIP_CLEAR_FAIL;
            }
        DO_PUSH:
            out.push_back(param.m_questIdx);
        SKIP_NULL_QUEST:;
        SKIP_CHECKQ_FAIL:;
        SKIP_CLEAR_FAIL:;
        }
        it++;
    }
}