// df_game_r Game/ CConditionEventManager（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CCONDITIONEVENTMANAGER_H
#define DNF_CCONDITIONEVENTMANAGER_H

class CConditionEventManager
{
public:
    CConditionEventManager();
    int GetCurEventIndex();  // ORIG 0x08335754
};

#endif  // DNF_CCONDITIONEVENTMANAGER_H