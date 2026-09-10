bool Quest::CheckKillMonster(int mapIdx, int dungeonIdx, int killCount,
                             ENUM_QUEST_ENEMY_TYPE enemyType,
                             std::vector<MonsterKillItem>& out) const
{
    // ORIG 0x83535dc：m_killItems 为空（无击杀要求）直接失败
    if (m_killItems.empty())
    {
        return 0;
    }
    out.reserve(m_killItems.size());
    for (int i = 0; i < (int)m_killItems.size(); ++i)
    {
        MonsterKillItem item = m_killItems[i];
        // ORIG 0x83535d6：4.1 if-join 岛形——前 3 个 && 条件失败各落一个
        // nop 中继岛（0x83536a9/ac/af），第 4 个失败直落尾块（0x83536b0）。
        // 4.4 复现形 = 负条件 goto 到物理连续尾标签串（P223 多岛细则）。
        if ((item.m_dungeonIdx != -1) && (item.m_dungeonIdx != dungeonIdx))
        {
            goto next_item1;
        }
        if ((item.m_mapIdx != -1) && (item.m_mapIdx != mapIdx))
        {
            goto next_item2;
        }
        // b28d-solo-Quest [P209]：enum 全非负 ⇒ 4.4 底层 unsigned、比较恒载寄存器；
        // ORIG 0x835367f `mov -0x2c(%ebp),%eax; cmp 0x18(%ebp),%eax`（mem 直比，有符号）
        // ⇒ 显式转 int 走 signed 比较。
        if (item.m_enemyType != (int)enemyType)
        {
            goto next_item3;
        }
        if (item.m_killCount == killCount)
        {
            out.push_back(item);
        }
    next_item1:;
    next_item2:;
    next_item3:;
    }
    if (out.size() == 0)
    {
        return 0;
    }
    return 1;
