#ifndef DNF_GAME_CDUNGEONCLEARTRACER_H_
#define DNF_GAME_CDUNGEONCLEARTRACER_H_

// df_game_r CDungeonClearTracer 还原（G2 副本链）。
// 布局 0x18：CBattle_Field 成员 @ +0x158..+0x170。
// ORIG：Reset 0830fece 全清零、Trace 0830fed32（cMyTrace 日志）。

class CDungeonClearTracer
{
public:
    void Trace(const char* name);        // ORIG 0x82fed32
    void SetMapId(int v);                // ORIG 0x82fee04
    void SetMaxBossCount(int v);         // ORIG 0x82fee12
    void SetKilledBossCount(int v);      // ORIG 0x82fee20
    void SetMaxMonsterCount(int v);      // ORIG 0x82fee2e
    void SetKilledMonsterCount(int v);   // ORIG 0x82fee3c
    void SetClearedDungeon(bool v);      // ORIG 0x82fee4a
    void SetEndPoint(bool v);            // ORIG 0x82fee62
    void IncrementKilledBossCount();     // ORIG 0x82fee7a
    void IncrementKilledMonsterCount();  // ORIG 0x82fee8e
    bool IsKilledBoss();                 // ORIG 0x82feea2（非 const）
    void Reset();                        // ORIG 0x82feece

private:
    int m_mapId;              // +0x00
    int m_maxBossCount;       // +0x04
    int m_killedBossCount;    // +0x08
    int m_maxMonsterCount;    // +0x0c
    int m_killedMonsterCount; // +0x10
    bool m_clearedDungeon;           // +0x14（ORIG 变参提升 movzbl）
    bool m_endPoint;                 // +0x15
};

static_assert(sizeof(CDungeonClearTracer) == 0x18, "CDungeonClearTracer size");

#endif  // DNF_GAME_CDUNGEONCLEARTRACER_H_
