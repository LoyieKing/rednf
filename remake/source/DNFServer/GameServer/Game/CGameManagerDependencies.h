#ifndef DNF_GAME_CGAME_MANAGER_DEPENDENCIES_H_
#define DNF_GAME_CGAME_MANAGER_DEPENDENCIES_H_

class CraneMinigameManager
{
public:
    CraneMinigameManager();
    ~CraneMinigameManager();
    void init();
    void updateCraneItemNeedMaterial(int);
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
