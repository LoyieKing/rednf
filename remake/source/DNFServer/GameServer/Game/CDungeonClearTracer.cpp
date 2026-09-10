// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDungeonClearTracer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CDungeonClearTracer（G2 副本链）。
// 逐函数对照 docs/class_func_reports/CDungeonClearTracer.md 与 ORIG 反汇编实现。
#include "CDungeonClearTracer.h"

#include "GameTypes.h"

#include "CDungeonClearTracer/SetMapId.inc"
{
    m_mapId = v;
}

#include "CDungeonClearTracer/SetMaxBossCount.inc"
{
    m_maxBossCount = v;
}

#include "CDungeonClearTracer/SetKilledBossCount.inc"
{
    m_killedBossCount = v;
}

#include "CDungeonClearTracer/SetMaxMonsterCount.inc"
{
    m_maxMonsterCount = v;
}

#include "CDungeonClearTracer/SetKilledMonsterCount.inc"
{
    m_killedMonsterCount = v;
}

#include "CDungeonClearTracer/SetClearedDungeon.inc"
{
    m_clearedDungeon = v;
}

#include "CDungeonClearTracer/SetEndPoint.inc"
{
    m_endPoint = v;
}

#include "CDungeonClearTracer/IncrementKilledBossCount.inc"
{
    m_killedBossCount = m_killedBossCount + 1;
}

#include "CDungeonClearTracer/IncrementKilledMonsterCount.inc"
{
    m_killedMonsterCount = m_killedMonsterCount + 1;
}

#include "CDungeonClearTracer/IsKilledBoss.inc"
{
    if (m_killedBossCount > 0 && m_killedBossCount >= m_maxBossCount)
    {
        return true;
    }
    return false;
}

#include "CDungeonClearTracer/Reset.inc"
{
    m_mapId = 0;
    m_maxBossCount = 0;
    m_killedBossCount = 0;
    m_maxMonsterCount = 0;
    m_killedMonsterCount = 0;
    m_clearedDungeon = 0;
    m_endPoint = 0;
}

#include "CDungeonClearTracer/Trace.inc"
{
    if (IsKilledBoss())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x108, 0)(
            "FUNC(%s) Map ID(%d), Boss(%d/%d), Monster(%d/%d) Cleared(%d), "
            "End Point(%d)\n",
            name, m_mapId, m_killedBossCount, m_maxBossCount,
            m_killedMonsterCount, m_maxMonsterCount, m_clearedDungeon,
            m_endPoint);
    }
    Reset();
}
