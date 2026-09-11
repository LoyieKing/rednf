// ============================================================================
// ConditionLevelChkDungeon::isEventDungeon —— ORIG T 0x089139d4
//   （报告 ConditionLevelChkDungeon/isEventDungeon.md）
//   m_unableDungeons 为空或不含 dungeonIdx → true。
// ============================================================================
// [b28c-31] AE：正向 `if (*it == dungeonIdx) return false;` 复现 ORIG
// 0x08913a2c 的 `sete %al; test; je <++it>` 正极性物化（46=46）。
bool ConditionLevelChkDungeon::isEventDungeon(int dungeonIdx)
{
    if (m_unableDungeons.empty())
    {
        return true;
    }
    std::vector<int>::iterator it = m_unableDungeons.begin();
    std::vector<int>::iterator endIt = m_unableDungeons.end();
    while (it != endIt)
    {
        // [b28c-31] ORIG 0x08913a2c：`cmp 0xc(%ebp),%eax; sete %al; test %al,%al;
        // je <++it>`（正极性 sete 物化）⇒ 正向 if 返回形，勿写 !(a==b)（4.4 折 setne）。
        if (*it == dungeonIdx)
        {
            return false;
        }
        ++it;
    }
    return true;
}