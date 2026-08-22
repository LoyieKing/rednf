// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 定义 SendLastMsgDBQueue（ORIG _ZN15CShutdowManager18SendLastMsgDBQueueEP5CUser）
// 及默认构造（GetInstanceShutdowManager 的 static 单例需要）。
// （注：上游单例 GetInstanceShutdowManager 由 CGameManager.cpp TU 提供。）

#include "CShutdowManager.h"

#include "CUser.h"

CShutdowManager::CShutdowManager()
{
}

void CShutdowManager::SendLastMsgDBQueue(class CUser* user)
{
    (void)user;
}

CShutdowManager* GetInstanceShutdowManager()
{
    static CShutdowManager s;
    return &s;
}