// ORIG reset 0x8562582：清玩家数组后 m_field00=0、m_field04=2。
// reset 置于 PlayerInfoThird::reset 定义前：callee 未知时 4.4 outgoing 保守
// 分配恰为 ORIG 帧 0x28（定义在后则继承 callee 小帧 0x14，槽号连带漂移）。
void TeamInfoThird::reset()
{
    for (int i = 0; i < 4; ++i)
        m_players[i].reset();
    m_field00 = 0;
    m_field04 = 2;
}