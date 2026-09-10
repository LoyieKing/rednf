MazeScript* CBattle_Field::GetAppropriateMaze(ENUM_DUNGEON_DIFFICULTY diff) const
{
    for (int questType = 0; questType < 2; ++questType)
    {
        std::vector<MazeScript>* mazes =
            m_pDungeon->GetQuestMazeScriptVector((ENUM_MAZE_QUEST_TYPE)questType);
        // b28d-solo-BattleDepsB2：ORIG 逐条结构（Ghidra C bVar2 汇聚 + 0x082ff128 段）：
        //   正向复合条件嵌套（血战/hell/连接/任务三条过滤），候选 size 三分支
        //   直接 return（无 picked/found 具名槽——ORIG 值在 %esi、标志在 %ebx）。
        if (mazes != NULL && !mazes->empty())
        {
            std::vector<MazeScript*> candidates;
            for (std::vector<MazeScript>::iterator it = mazes->begin();
                 it != mazes->end(); ++it)
            {
                MazeScript* maze = &*it;
                if ((int)m_bloodState != 1 &&             // +0x1b4
                    ((char)m_hellPartyActive == 0 || maze->m_70 == 0))
                {
                    if (m_party != NULL &&
                        m_party->CheckQuestConnection(
                            maze->m_50, (ENUM_MAZE_QUEST_TYPE)questType))
                    {
                        Quest* quest = G_CDataManager()->find_quest(maze->m_50);
                        if (quest == NULL || quest->m_field120 != 1 ||
                            m_party->get_member_count() < 2)
                        {
                            if (questType == 0)
                            {
                                if (maze->m_54 <= (int)diff)
                                    // b28d-55（P270/P234）：ORIG 走
                                    // push_back(T&&)=EOS1_（0x082ff2a4 实证）；
                                    // static_cast<T&&> 是纯语言构造（零额外 call）
                                    // 且选中同一右值重载，P167 差集 25/25 CLEAR。
                                    candidates.push_back(static_cast<MazeScript*&&>(maze));
                            }
                            else
                            {
                                candidates.push_back(static_cast<MazeScript*&&>(maze));
                            }
                        }
                    }
                }
            }

            // b28d-55（P167 差集实证）：ORIG 三次独立 size() 调用
            // （`==1`→sete / `>1`→seta / `%size()`→结果缓存 -0x3c 槽），
            // 无 cnt 具名缓存。
            if (candidates.size() == 1)
                return candidates[0];
            if (candidates.size() > 1)
                return candidates[::rand() % candidates.size()];
        }
    }
    return (MazeScript*)m_pDungeon->GetRandMazebyPartyCount(m_party->get_member_count());
