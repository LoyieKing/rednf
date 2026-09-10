void QuestList::GetDailyQuestList(std::list<int>& out)
{
    // [b28d-solo-CListTables] ORIG 0x0835589e：it 先 C1Ev 默认构造（-0x24）再
    // `it = begin()`（sret -0x3c 拷贝）、quest 函数头清零（-0xc，P171 死初始化）、
    // range 先 C1Ev（-0x34）再 `range = equal_range(...)`（临时 -0x1c + op=）、
    // cit 初始化式（-0x38）、it++/cit++ 均后置（ppEi，临时 -0x10/-0x14）。
    std::list<int>::iterator it;
    Quest* quest = 0;
    // [b28d-solo-CListTables P212/P309] ORIG 槽深序 quest(-0xc)/it(-0x24)/
    // npcIdx(-0x28)/questIdx(-0x2c)/range(-0x34)/cit(-0x38) ⇒ 全头部声明、
    // 声明序按 ORIG 浅→深；块内仅赋值。
    int npcIdx;
    int questIdx;
    std::pair<std::multimap<int, int>::const_iterator,
              std::multimap<int, int>::const_iterator> range;
    it = m_listE0.begin();
    while (it != m_listE0.end())
    {
        npcIdx = *it;
        range = m_npcQuestList7.equal_range(npcIdx);
        std::multimap<int, int>::const_iterator cit = range.first;
        while (cit != range.second)
        {
            questIdx = cit->second;
            quest = find_quest(questIdx);
            // [b28d-solo-CListTables] ORIG 0x0835597e/0x08355981：两守卫落点不同
            // （+0xe0 nop;jmp / +0xe3 nop）⇒ P223 多岛形：独立否定 goto + 尾标签串。
            if (quest == 0)
                goto SKIP_NULL_QUEST;
            if (quest->m_field8 != 3)
                goto SKIP_NOT_DAILY;
            out.push_back(questIdx);
        SKIP_NULL_QUEST:;
        SKIP_NOT_DAILY:;
            cit++;
        }
        it++;
    }
