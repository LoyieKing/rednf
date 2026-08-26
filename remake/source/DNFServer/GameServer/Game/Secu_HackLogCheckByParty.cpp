#include "Secu_HackLogCheckByParty.h"
#include "CParty.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CMap.h"
#include "CInventory.h"
#include "LogManager.h"
// ============================================================================
// df_game_r Secu_HackLogCheckByParty 还原（G2-5 批次，自包含 TU）
// 逐函数对照 docs/class_func_reports/Secu_HackLogCheckByParty.md 与 ORIG 反汇编。
// 布局（ORIG ctor/dtor/Init 推导，sizeof 0x14，CParty +0x1abc）：
//   +0x00 Secu_DungeonAverageCheck 基类（0x10：CParty* + vector 占位）
//   +0x10 CParty* m_party2
// 类型声明统一于 Secu_HackLogCheckByParty.h（唯一声明点）。
// ============================================================================

// ============================================================================
// 实现
// ============================================================================

Secu_HackLogCheckByParty::Secu_HackLogCheckByParty()
{
    m_party2 = 0;
}

Secu_HackLogCheckByParty::~Secu_HackLogCheckByParty()
{
}

void Secu_HackLogCheckByParty::Init(CParty* party)
{
    m_party2 = party;
    Secu_DungeonAverageCheck::SetPartyPtr(party);
}

void Secu_HackLogCheckByParty::DieMob(const MSG_MONSTER_DIE* msg,
                                      map_monster* mob)
{
    if (msg->m_fieldF != -1)
    {
        Secu_DungeonAverageCheck::setKillMonsterInfo(msg, mob);
    }
}

void Secu_HackLogCheckByParty::CheckMoveMapFitInPathHackCnt(
    CUser* user, CParty* party, unsigned int a, unsigned int b, int mapIdx)
{

    // P15/P16：ORIG 将 (auto||cnt==1)&&map&&!fit 结果物化为 0/1 再统一 test；
    // 参数判空次序按 ORIG：先 party(0x10) 后 user(0xc)。
    if (party == 0 || user == 0)
    {
    }
    else if (!party->IsFirstMapClear())
    {
        party->SetFirstMapClear(true);
    }
    else
    {
        CMap* map = (CMap*)G_CDataManager()->find_map(mapIdx);

        if ((party->IsAutoCreated() || party->get_member_count() == 1)
            && map != 0
            && !map->checkFitInPathObject(a, b))
        {
            LogManager::logFormat(
                1, "Secu_HackLogCheckByParty.cpp",
                "void Secu_HackLogCheckByParty::CheckMoveMapFitInPathHackCnt(CUser*, CParty*, unsigned int, unsigned int, int)",
                0x2c, "Path gate position error. (User: %s %d,%d)",
                NumberToString(user->get_acc_id(), 0), a, b);
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1f7, 1, a, b);
        }
    }
}


void Secu_HackLogCheckByParty::startDungeon()
{
    Secu_DungeonAverageCheck::startDungeon();
}

void Secu_HackLogCheckByParty::dungeonClear()
{
    Secu_DungeonAverageCheck::dungeonClear();
}

void Secu_HackLogCheckByParty::dungeonFail()
{
    Secu_DungeonAverageCheck::dungeonFail();
}
