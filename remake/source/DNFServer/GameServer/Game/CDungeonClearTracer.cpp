// df_game_r 还原 —— CDungeonClearTracer（G2 副本链）。
// 逐函数对照 docs/class_func_reports/CDungeonClearTracer.md 与 ORIG 反汇编实现。
#include "CDungeonClearTracer.h"

#include "GameTypes.h"

void CDungeonClearTracer::SetMapId(int v)
{
    m_mapId = v;
}

void CDungeonClearTracer::SetMaxBossCount(int v)
{
    m_maxBossCount = v;
}

void CDungeonClearTracer::SetKilledBossCount(int v)
{
    m_killedBossCount = v;
}

void CDungeonClearTracer::SetMaxMonsterCount(int v)
{
    m_maxMonsterCount = v;
}

void CDungeonClearTracer::SetKilledMonsterCount(int v)
{
    m_killedMonsterCount = v;
}

void CDungeonClearTracer::SetClearedDungeon(bool v)
{
    m_clearedDungeon = v;
}

void CDungeonClearTracer::SetEndPoint(bool v)
{
    m_endPoint = v;
}

void CDungeonClearTracer::IncrementKilledBossCount()
{
    m_killedBossCount = m_killedBossCount + 1;
}

void CDungeonClearTracer::IncrementKilledMonsterCount()
{
    m_killedMonsterCount = m_killedMonsterCount + 1;
}

bool CDungeonClearTracer::IsKilledBoss()
{
    return m_killedBossCount > 0 && m_killedBossCount >= m_maxBossCount;
}

void CDungeonClearTracer::Reset()
{
    m_mapId = 0;
    m_maxBossCount = 0;
    m_killedBossCount = 0;
    m_maxMonsterCount = 0;
    m_killedMonsterCount = 0;
    m_clearedDungeon = 0;
    m_endPoint = 0;
}

void CDungeonClearTracer::Trace(const char* name)
{
    if (IsKilledBoss())
    {
        cMyTrace("void CDungeonClearTracer::Trace(const char*)", 0x108, 0)(
            "FUNC(%s) Map ID(%d), Boss(%d/%d), Monster(%d/%d) Cleared(%d), "
            "End Point(%d)\n",
            name, m_mapId, m_killedBossCount, m_maxBossCount,
            m_killedMonsterCount, m_maxMonsterCount, m_clearedDungeon,
            m_endPoint);
    }
    Reset();
}
