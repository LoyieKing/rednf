// ORIG 0x082ff128 GetAppropriateMaze(diff)：
//   按 questType 0/1 取任务迷宫向量；过滤血战态/hell 标记/任务连接/单人格挡；
//   questType==0 额外要求 maze->m_54 <= diff；候选 1 个直取、多个 rand()%n，
//   两轮无候选则回退 CDungeon::GetRandMazebyPartyCount(成员数)。
// b28d-solo-BattleDepsB2 定案（N 封顶 217/212）：已对齐项 = 正向复合条件嵌套（血战/hell/
//   连接/任务三过滤）、候选 size 三分支直接 return（无 picked/found 具名槽）、
//   push_back 右值重载、size() 三次独立调用；p167 CLEAR(25/25)。残差 = 4.1/4.4
//   槽序（maze 等局部 N82/N98）+ `get_member_count()>1` 的 setg 物化
//   （ORIG cmp/setg/test vs 4.4 直跳，P197④ 族）⇒ 封顶。
// b28e 复核（gap=5 成因定案）：5 条缺口**全部**是循环体尾的 3 个 if-join 空岛
//   （ORIG +0x19d `nop;jmp +0x1a4`、+0x1a0 `nop;jmp +0x1a4`、+0x1a3 `nop`；
//   0x82ff2c5/0x82ff2c8 两岛由血战 je / hell jne 落达，第三岛直落 +0x1a4）。
//   4.4 将三级失败路径全部 jump-thread 到循环尾 ⇒ 0 岛。P223 负条件 goto 法
//   仅适用「各假路汇**同一**尾标签」；本件 ORIG 各岛落点**不同址**（多级早返
//   分级），P223 边界负例（同级实测量不可择）⇒ 不翻。其余为槽基/槽序（N82/N95）。
//   语义侧：p167 25/25 CLEAR，无缺口。
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
                // [B-lane 试形 Y] P223(BM8) 多岛尾标签串：ORIG 三个 if-join 空岛
                //   （+0x19d/+0x1a0 nop;jmp +0x1a4、+0x1a3 nop 直落）在 4.4 被
                //   jump-thread 折叠 ⇒ 用 2 守卫 + 3 尾标签复现。
                if ((int)m_bloodState == 1)                       // +0x1b4
                    goto skip1;
                if ((char)m_hellPartyActive != 0 && maze->m_70 != 0)
                    goto skip2;
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
skip3:;
skip2:;
skip1:;
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
}