int UserQuest::CheckDungeonEventStoryPause(int dungeonNo, int fieldNo, int mapNo) const
{
    // b28d-40: 两个 continue 改嵌套正向 if 包裹（消 4.4 同层多 continue 的 nop+jmp
    // 中继岛，ORIG 0x086ad079/0x086ad0a7 两守卫直接落外层循环尾 0x086ad159）。
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            // P167：ORIG 0x086ad0xx 三处调 const 版
            // `_ZNKSt6vectorI24dungeonEventHanaseStructSaIS0_EEixEj` ⇒ 原码 quest 为 const Quest*。
            const Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest != 0)
            {
                for (unsigned int j = 0; j < quest->m_eventDialogs.size(); ++j)
                {
                    switch (quest->m_eventDialogs[j].m_field0)
                    {
                    case 0:
                        if (quest->m_eventDialogs[j].m_field8 == fieldNo)
                        {
                            return 1;
                        }
                        break;
                    case 1:
                        if (quest->m_eventDialogs[j].m_field8 == dungeonNo)
                        {
                            return 1;
                        }
                        break;
                    }
                }
            }
        }
    }
    return 0;
}