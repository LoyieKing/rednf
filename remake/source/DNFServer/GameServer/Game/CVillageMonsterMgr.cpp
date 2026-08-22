// df_game_r 还原 —— village_attacked::CVillageMonsterMgr
// 独立 TU（从 GameStubs.cpp 拆出）。只定义本 batch 的三个方法；
// OnKillVillageMonster 由 VillageMonster_helpers.cpp 提供（不入本批）。
// 方法当前为空实现（对应 GameStubs.cpp 的 TODO(G3) 桩），待有报告证据后补真实语义。

#include "CVillageMonsterMgr.h"

namespace village_attacked
{

void CVillageMonsterMgr::OnMoveArea(CUser*)
{
    // TODO(G3)
}

void CVillageMonsterMgr::OnEvent()
{
    // TODO(G3)
}

void CVillageMonsterMgr::InsertVillageMonster(int, int, int, int,
                                              std::vector<STAttackedMonster>*,
                                              std::vector<MapArea>*)
{
    // TODO(G3)
}

}  // namespace village_attacked