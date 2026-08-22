#ifndef DNF_GAME_CRANE_MINIGAME_MANAGER_H_
#define DNF_GAME_CRANE_MINIGAME_MANAGER_H_

// CraneMinigameManager —— 从 GameStubs.cpp 迁移（ORIG 0x080ebc40 起，仅声明的桩方法）
class CraneMinigameManager
{
public:
    CraneMinigameManager();
    ~CraneMinigameManager();
    bool init();                      // ORIG 0x080ebc6e 返回 1 成功 / 0 失败
    void updateCraneItemNeedMaterial(int count);
};

#endif  // DNF_GAME_CRANE_MINIGAME_MANAGER_H_