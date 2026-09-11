// [b28d-solo-CListTables AE] ORIG 0x08355304：113=113——①quest=0 声明提到
// out.clear() 之前（ORIG `movl $0x0,-0xc` 先于 clear）；②`return !out.empty()`
// 拆成 `if (out.empty()) return false; return true;`（P56，消 4 条 bool 物化）；
// ③questIdx 全头部声明（P309/P212，it/questIdx 槽序对齐）。
bool QuestList::select_mail_quest(std::list<int>& out,
                                  const WongWork::CQuestClear& clear,
                                  const stSelectQuestParam& param)
{
    // [b28d-solo-CListTables] ORIG 0x08355304：quest 函数头清零（-0xc）、迭代器为
    // const_iterator（begin() 结果走 C1<Pi> 转换构造）、每条件独立 guard
    // （`xor $1; test; jne fail`，P133/P168a 不可合并 && 短路）。
    Quest* quest = 0;
    int questIdx;
    out.clear();
    for (std::vector<int>::const_iterator it = m_vecD4.begin();
         it != m_vecD4.end(); ++it)
    {
        questIdx = *it;
        quest = find_quest(questIdx);
        if (quest == 0)
            continue;
        if (!quest->check_possible(param))
            continue;
        if (!check_clear(questIdx, clear))
            continue;
        if (!check_ahead_quest(questIdx, clear))
            continue;
        if (!check_anti_quest(questIdx, clear))
            continue;
        out.push_back(questIdx);
    }
    if (out.empty())
        return false;
    return true;
}