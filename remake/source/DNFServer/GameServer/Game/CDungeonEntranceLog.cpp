// df_game_r Game/ CDungeonEntranceLog 实现（批3：从 GameStubs.cpp 迁移）。
#include "CDungeonEntranceLog.h"

void CDungeonEntranceLog::DecrementDungeonEntrance(int, bool)
{
    // TODO(G2)
}

void CDungeonEntranceLog::IncrementDungeonEntrance(int, bool)
{
    // TODO(G2)
}

CDungeonEntranceLog* GetInstanceDungeonEntranceLog()
{
    static CDungeonEntranceLog s;
    return &s;
}