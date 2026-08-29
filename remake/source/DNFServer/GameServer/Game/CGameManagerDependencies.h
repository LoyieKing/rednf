#ifndef DNF_GAME_CGAME_MANAGER_DEPENDENCIES_H_
#define DNF_GAME_CGAME_MANAGER_DEPENDENCIES_H_

class CUser;
class CraneMinigameManager
{
public:
    CraneMinigameManager();
    ~CraneMinigameManager();
    bool init();
};

class CConditionEventManager
{
public:
    CConditionEventManager();
    ~CConditionEventManager();
    int GetCurEventIndex();
};

class CSpecialItemRoutingManager
{
public:
    CSpecialItemRoutingManager();
    ~CSpecialItemRoutingManager();
};

#endif
