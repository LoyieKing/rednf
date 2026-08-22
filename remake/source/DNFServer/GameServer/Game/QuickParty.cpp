// df_game_r 还原 —— QuickParty 独立实现（batch-5 迁移自 GameStubs.cpp）。
// 仅包含本批范围内方法；CQuickPartyRewardManager 不在本批，不在此实现。
// ORIG 符号见 QuickParty.h 头部注释。

#include "QuickParty.h"

namespace QuickParty
{

// ===================== CQuickParty =====================

CQuickParty::CQuickParty() {}
CQuickParty::~CQuickParty() {}

int CQuickParty::get_quick_party_index() const { return 0; }
void CQuickParty::init() {}
void CQuickParty::set_quick_party_index(int) {}

// ===================== CQuickPartySystemManager =====================

CQuickPartySystemManager::CQuickPartySystemManager() {}
CQuickPartySystemManager::~CQuickPartySystemManager() {}

void CQuickPartySystemManager::init_quick_party_pool(const CDungeon*) {}
void CQuickPartySystemManager::cancel_quick_party(CParty*, bool, CUser*) {}

}  // namespace QuickParty