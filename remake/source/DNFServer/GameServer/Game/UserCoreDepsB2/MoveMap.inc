// ---------------------------------------------------------------------------
// 5a. Secu_HackLogCheck::MoveMap —— ORIG T 0x0827a7f4
// （报告 Secu_HackLogCheck/MoveMap.md）：三检查使能位置 1 后调基类 Reset。
// ---------------------------------------------------------------------------
void Secu_HackLogCheck::MoveMap()
{
    m_flag223 = 1;
    m_flag224 = 1;
    m_flag225 = 1;
    Reset();
}