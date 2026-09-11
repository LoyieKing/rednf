// ORIG 0x0830107e：四段直写 m_dungeonClearTracer。
// [b28d-D9] N95：ORIG 4.1 dead 局部 -0x4(%ebp) 单条 store；4.4 槽基恒 ebp-8（P232/P190 均不可上移），残 1 条（ORIG 0x0830107e sz 0x0000006f）
// [S6 2026-09-11] 残差 N（帧）：ORIG 0x08301081 `sub $0x18,%esp` + `movl $0x0,-0x4(%ebp)`
// vs 我方 0x28/%ebp-0xc（dummy 槽 4.4 恒 ebp-0xc、帧保守值 0x28）⇒ 2 处操作数差，
// 助记符序全等（NEAR 已达成）；4.4 无源侧旋钮可抬槽基 ⇒ 判 N95/N82。
void CBattle_Field::setDungeonClearTrace(int mapId, int maxBoss, int killedBoss,
                                         int killedMonster)
{
    m_dungeonClearTracer.SetMapId(mapId);
    m_dungeonClearTracer.SetMaxBossCount(maxBoss);
    m_dungeonClearTracer.SetKilledBossCount(killedBoss);
    m_dungeonClearTracer.SetKilledMonsterCount(killedMonster);
    // ORIG 0x083010e4 尾部 `movl $0x0,-0x4(%ebp)`（4.1 dead 局部顶到 ebp-4）。
    int dummy = 0;
    (void)dummy;
}