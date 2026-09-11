// ORIG 0x0830e170 setQuestMazeInfo()：扫描两类任务迷宫，符合条件者置任务迷宫标记。
// b28d-solo-BattleDepsB2 定案（NEAR 117/117，N-P75 封顶）：已对齐 = for(i<=1) 尾测 setle、
//   `mazes!=NULL && !empty()` 正向包裹、while(it!=itEnd) 三槽阶梯、continue 中继
//   nop 岛、全部失败路径汇入循环尾 ++it（ORIG +0x154）。残差仅 2 处 receiver/
//   实参装载序（ORIG 先 receiver 后实参；4.4 恒先算实参，probe_ord2 f1..f10
//   十形全同，4.4.6 后端读数逐字节一致）⇒ 跨版本装载时刻，封顶。
// [b28e 复核] 117=117 不变；两处残差定位 = ①`GetQuestMazeScriptVector(questType)`
//   调用（ORIG 先 receiver 后实参 vs 4.4 先实参）、②`CheckQuestConnection(maze->m_50,
//   questType)`（ORIG arg1→receiver→arg2 vs 4.4 arg2→arg1→receiver）；均属 4.1/4.4
//   实参装载时刻（P75），源形不可择 —— 维持封顶，无新增手法。
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
}