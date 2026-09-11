// [b28d-32 N(P75/N93): 实参装载序与槽位差（94/86） | ORIG 0x08354fc8]
bool QuestList::check_ahead_quest(int questIdx,
                                 const WongWork::CQuestClear& clear)
{
    Quest* quest = find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__,
            0x14b7, "find_quest(%d)", questIdx);
        return 0;
    }
    // [b28d-solo-CListTables] ORIG 0x08354fc8：empty() 正形 guard（call empty;
    // test; je 循环；非空 fall-through 到 return 1）+ 外层 size 具名局部 -0x1c +
    // 内层 size 具名局部 -0x14（P157 每次重读 operator[]）。
    if (quest->m_vec2c.empty())
    {
        return 1;
    }
    // [b28d-solo-CListTables] ORIG 0x08355042：empty 检查后无条件发射一次
    // __normal_iterator<int*,vector<int>>::C1Ev（-0x28 槽），全函数未再使用
    // ⇒ 原码存在未使用的迭代器局部（P171 照抄死写入/P232 死局部占帧）。
    std::vector<int>::iterator unusedIt;
    unsigned int aheadIdx;
    unsigned int size1 = quest->m_vec2c.size();
    for (unsigned int i = 0; i < size1; ++i)
    {
        unsigned int size2 = quest->m_vec2c[i].size();
        // [b28d-solo-CListTables] P158：ORIG `movzbl found; xor $1; test; je`
        // ⇒ found 为 bool 且条件写 !found（char+!=1 在 4.4 折 cmpb $1）。
        bool found = false;
        for (unsigned int j = 0; j < size2; ++j)
        {
            aheadIdx = quest->m_vec2c[i][j];
            if (!clear.isClearedQuest(aheadIdx))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return 1;
        }
    }
    return 0;
}