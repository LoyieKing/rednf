// ============================================================================
// df_game_r 还原 —— CraneMinigameManager（从 GameStubs.cpp 迁移，G2）
// ORIG 符号（nm -C df_game_r）：
//   _ZN20CraneMinigameManagerC1Ev                     CraneMinigameManager()
//   _ZN20CraneMinigameManagerD1Ev                     ~CraneMinigameManager()
//   _ZN20CraneMinigameManager4initEv                  init()
//   _ZN20CraneMinigameManager27updateCraneItemNeedMaterialEi
// init() ORIG 0x080ebc6e：importCraneMinigameScript 成功后置 1、失败置 0，
// 并调用 Action_SendMail::reset（this+0x18）。当前为占位实现（返回 1），
// 后续补 import/send-mail 语义。符号 T。
// ============================================================================
#include "CraneMinigameManager.h"

CraneMinigameManager::CraneMinigameManager()
{
    // TODO(G2): ORIG 0x080ebc40 —— 成员初始化。
}

CraneMinigameManager::~CraneMinigameManager()
{
    // TODO(G2): ORIG 0x080ebc5a —— 成员析构。
}


bool CraneMinigameManager::init()
{
    // ORIG 0x080ebc6e：importCraneMinigameScript(script) 成功返回 1、失败返回 0，
    // 并重置 Action_SendMail（this+0x18）。占位实现返回 1（成功）。
    // TODO(G2): 补 importCraneMinigameScript / Action_SendMail::reset。
    return true;
}

void CraneMinigameManager::updateCraneItemNeedMaterial(int count)
{
    // ORIG 0x080ebebc：更新所需材料数。占位实现。
    // TODO(G2)
    (void)count;
}