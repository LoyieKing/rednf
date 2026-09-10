void QuestList::insert_NPC(int npcIdx)
{
    // [b28d-32] ORIG 0x08354f02：`C1Ev` 默认构造迭代器 + 后置 `it++`（ppEi）+
    // 双 nop 岛（循环内 return 与正常出口汇合）⇒ 先声明后赋值（P50 复活）、
    // while 形、后置增量。
    if (npcIdx == 0)
        return;
    std::list<int>::iterator it;
    it = m_listE0.begin();
    while (it != m_listE0.end())
    {
        if (*it == npcIdx)
        {
            return;
        }
        it++;
    }
    m_listE0.push_back(npcIdx);
