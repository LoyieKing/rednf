// df_game_r Game/ CDungeonEntranceLog（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CDUNGEONENTRANCELOG_H
#define DNF_CDUNGEONENTRANCELOG_H

class CDungeonEntranceLog
{
public:
    void DecrementDungeonEntrance(int a, bool b);
    void IncrementDungeonEntrance(int a, bool b);
};

CDungeonEntranceLog* GetInstanceDungeonEntranceLog();

#endif  // DNF_CDUNGEONENTRANCELOG_H