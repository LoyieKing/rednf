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
