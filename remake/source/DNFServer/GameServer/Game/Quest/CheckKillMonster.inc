// b28d-solo-Quest 判词 N18b 封顶：ORIG 0x083535d6。P223 多岛复现成功（3 个
// nop+jmp 岛 + 尾 nop，与 ORIG 0x83536a9/ac/af 逐址对应）。残 1 条 `mov`：
// ORIG `mov -0x2c(%ebp),%eax; cmp 0x18(%ebp),%eax`（mem 直比），4.4 对 enum
// 形参比较恒载寄存器 `mov -0x2c,%edx; mov 0x18(%ebp),%eax; cmp %eax,%edx`。
// 本轮 18 形探针（无 cast/(int)/(ENUM) cast/交换/取址视图/一元+ 等，4.4.7 -m32
// -O0）全败；同函数 int 形参（mapIdx/dungeonIdx/killCount）比较已 mem 直比 ⇒
// 差异根因=ENUM_QUEST_ENEMY_TYPE 全非负 ⇒ 4.4 底层 unsigned、比较走寄存器。
// [F2e-Singles2 C05② 复核 2026-09-10] 4 处比较对象全为**形参**（0xc/0x10/0x14/0x18
//   (%ebp)，含 ENUM_QUEST_ENEMY_TYPE）⇒ C05②「两个具名局部比较」命中形**不成立**
//   （边界表 ❌ 形参子型），维持 N18b（18 形探针已穷举）。our 94 vs ORIG 93（gap −1）。
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
}