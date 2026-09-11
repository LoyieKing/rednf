// b28d-solo-CHackAnalyzer: AE（窗口检查改正向包裹 `if (<= 0xa) {...}`，
// ORIG 0x080f7eb6 双出口岛形逐条对齐；旧 P223 边界判词作废）。
void CHackAnalyzer::_analyzeMonsterDieHack()
{
    // ORIG 0x080f7eb6（_analyzeMonsterDieHack.md）：采集窗口 <11 秒且 kill 记录
    // 全为 0xffff 填充时记 0x3c0。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_MONSTER_DIE))
        return;

    // b28d-solo-CHackAnalyzer: 窗口检查改正向包裹（P222/P59 补强）——ORIG
    // 0x080f7eec `jg 0x119(leave)` 直落共享出口（无岛），而 checkCollectedHack
    // 的早退走 0x118 单个 nop 岛；早返形会让 4.4 给两个出口各发 [nop;jmp]
    // （87→89）。包裹后窗口出口直跳 leave、早退保留单岛，逐条对齐。
    if (m_field24 - m_field20 <= 0xa)
    {
        std::vector<int>::const_iterator it = m_checkTypes.begin();
        std::vector<int>::const_iterator e = m_checkTypes.end();
        while (it != e)
        {
            if (*it != 0xffff)
                break;      // ORIG body 形：setne+test+jne 退出（等价 && 短路）
            ++it;
        }
        if (it == e)
        {
            _addHackInfo(REPORT_4_HACK_FLAG_MONSTER_DIE, (ENUM_HACKTYPE)0x3c0,
                         m_field24 - m_field20);
        }
    }
}