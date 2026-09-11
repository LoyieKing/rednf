// ORIG C1/C2 0x85668ff0（W）：仅对 4 个 PlayerInfoThird 子对象做默认构造
// （esi=3 递减循环 call PlayerInfoThird::C1）；不动 m_field00/m_field04
// （由 reset 显式置值）。编译器自动为 m_players[4] 逐个调用 PlayerInfoThird()。
TeamInfoThird::TeamInfoThird()
{
}