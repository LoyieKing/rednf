void CBattle_Field::setQuestMazeInfo()
{
    // b28d-solo-BattleDepsB2（ORIG 0x0830e170 逐条）：外层 `for(i=0;i<=1;++i)`（尾测 setle，P2）；
    //   `mazes!=NULL && !empty()` 正向包裹（ORIG +0x02e/+0x03f xor$1 双条件共享
    //   失败块）；内层 while(it!=itEnd)（入口 jmp 到 +0x15f 尾测，P25）；所有失败
    //   路径汇入循环尾 `++it`（+0x154）；连接判定直接 `if (m_party!=NULL && call)`
    //   正向包裹（ORIG +0xa9..+0xe0 bool 物化岛）。
    for (int questType = 0; questType <= 1; ++questType)
    {
        std::vector<MazeScript>* mazes =
            m_pDungeon->GetQuestMazeScriptVector((ENUM_MAZE_QUEST_TYPE)questType);
        if (mazes != NULL && !mazes->empty())
        {
            std::vector<MazeScript>::iterator it = mazes->begin();
            std::vector<MazeScript>::iterator itEnd = mazes->end();
            for (; it != itEnd; ++it)
            {
                MazeScript* maze = &*it;
                if ((int)m_bloodState == 1)
                {   // 血战态跳过（ORIG je 落 +0x153 nop 中继岛）
                    continue;
                }
                if (m_party != NULL &&
                    m_party->CheckQuestConnection(
                        maze->m_50, (ENUM_MAZE_QUEST_TYPE)questType))
                {
                    Quest* quest = G_CDataManager()->find_quest(maze->m_50);
                    if (quest != NULL)
                    {
                        if (quest->m_field120 == 1)
                        {   // 单人限定任务迷宫：仅单人队伍置位
                            if (m_party->get_member_count() == 1)
                                setQuestMaze(true);
                        }
                        else
                        {
                            setQuestMaze(true);
                        }
                    }
                }
            }
        }
    }
