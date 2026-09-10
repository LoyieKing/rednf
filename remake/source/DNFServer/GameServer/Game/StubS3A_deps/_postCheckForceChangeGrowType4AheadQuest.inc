void _postCheckForceChangeGrowType4AheadQuest(CUser* user, Quest* quest)
{
    if (quest->m_intDataList.empty())   // ORIG 0x086686f4：mov 0xc(%ebp);add $0x2c;call empty
        return;
    // ORIG 0x086870a：empty 检查后 `lea -0x1c; call normal_iterator::C1Ev`（P50 先声明）
    std::vector<int>::iterator it;
    // P212：ORIG 槽序 gi(-0x10) 浅于 questIdx(-0x14) ⇒ 全头部声明、questIdx 先声明（深）。
    int questIdx;
    for (unsigned int gi = 0; gi < quest->m_intDataList.size(); ++gi)
    {
        for (it = quest->m_intDataList[gi].begin();
             it != quest->m_intDataList[gi].end(); ++it)
        {
            questIdx = *it;
            Quest* q = G_CDataManager()->find_quest(questIdx);
            if (q == 0)
                continue;
            _postCheckForceChangeGrowType4AheadQuest(user, q);
            // ORIG 0x086878f：`mov -0x14,%ebx` 先物化 questIdx，再算 receiver（单语句内联）。
            reinterpret_cast<WongWork::CQuestClear*>(
                reinterpret_cast<char*>(user->getCurCharacQuestW()) + 4)
                ->setClearedQuest((unsigned int)questIdx);
        }
    }
