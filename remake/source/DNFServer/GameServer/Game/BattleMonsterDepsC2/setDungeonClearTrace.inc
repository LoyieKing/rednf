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
