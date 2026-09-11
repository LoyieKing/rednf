// [b28d-32 N(P75/N93): 栈拷贝/装载序与分支块序差（88/79） | ORIG 0x0835511e]
bool QuestList::check_anti_quest(int questIdx,
                                const WongWork::CQuestClear& clear)
{
    Quest* quest = find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__,
            0x150a, "find_quest(%d)", questIdx);
        return 0;
    }
    // [b28d-solo-CListTables] ORIG 0x0835511e：具名迭代器先 C1Ev 默认构造（-0x2c）
    // 再 `it = begin()`（sret 临时 -0x3c，P50 复活）+ while 每轮调 end()（-0x28）
    // + 尾后置 `it++`（ppEi 临时 -0x14，P28）；antiIdx 为 int（-0xc，ORIG
    // `cmpl $0x752f; jle` 有符号，P196）。
    std::vector<int>::iterator it;
    it = quest->m_antiQuestList.begin();
    while (it != quest->m_antiQuestList.end())
    {
        int antiIdx = *it;
        if (antiIdx > 29999)
        {
            // [b28d-solo-CListTables] P221：全表达式临时链翻 sibling/具名对象槽方向
            // （ORIG trace 槽 -0x24 浅于 it -0x2c；具名形 4.4 恒相反）。
            cMyTrace(__PRETTY_FUNCTION__, 0x151a, 5)(
                "[%s][%d]", __PRETTY_FUNCTION__, 0x151a);
            return 0;
        }
        if (clear.isClearedQuest(antiIdx))
        {
            return 0;
        }
        it++;
    }
    return 1;
}