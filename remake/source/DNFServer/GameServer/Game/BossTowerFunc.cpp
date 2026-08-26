// ============================================================================
// BossTowerFunc.cpp —— Global::CBossTowerFunc::{UseItem,SelectDungeon} +
// CUser::getBossTower（跨类叶子助手，与调用方同 TU 承载，避免触碰高并发
// 编辑的 CUser.cpp）。
//
// 证据：docs/class_func_reports/Global__CBossTowerFunc/{UseItem,SelectDungeon}.md、
// docs/class_func_reports/CUser/getBossTower.md（ORIG 汇编+反编译 C 全文已读）。
// 阻塞符号（声明就位、定义待批次）：CBossTower::handleStartGame/handleUseItem。
// ============================================================================
#include "BossTowerFunc.h"

#include "CGameManager.h"   // G_CGameManager / getBossTower() / getBossTower(int)
#include "CBossTower.h"     // WongWork::CBossTower（handleStartGame/_destroy/handleUseItem）
#include "CParty.h"         // CParty::useItem / get_member_count（完整类型）
#include "Inven_Item.h"     // Inven_Item 完整布局（m_field1/m_addInfo 字段化读取）
#include "GameTypes.h"      // cMyTrace

#include "CUser.h"          // CUser 完整类型（get_state/GetParty/getBossTower）

// ----------------------------------------------------------------------------
// ORIG T 0x08146c20 _ZN6Global14CBossTowerFunc7UseItemEP5CUserRK10Inven_Item
// state==5(队伍玩法) → CParty::useItem；state==12(0xc,BossTower) →
// CBossTower::handleUseItem(item.m_addInfo(+0x02), item.m_field1(+0x01))。
// 两分支均返回 1；其它 state 返回 0。
// ----------------------------------------------------------------------------
bool Global::CBossTowerFunc::UseItem(CUser* user, const Inven_Item& item)
{
    const int state = user->get_state();
    if (state == 5)
    {
        // ORIG _ZN5CUser8GetPartyEv 返回 CParty*；现头声明为 void*，此处语义转换
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            party->useItem(user, item);
        }
        return true;
    }
    if (state == 0xc)
    {
        WongWork::CBossTower* tower = user->getBossTower();
        if (tower != 0)
        {
            tower->handleUseItem(user,
                                 static_cast<unsigned long>(item.m_addInfo),  // +0x02 dword
                                 item.m_field1);                              // +0x01 ITEM_TYPE byte
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146722 _ZN6Global14CBossTowerFunc13SelectDungeonEP6CPartys17ENUM_DUNGEON_TYPE
// party 判空 → 0；getBossTower() 判空 → cMyTrace(PRETTY_FUNCTION,0x34,5) +
// "StaticPool BossTower NULL" → 0；按 get_member_count 映射 (1→0,2→1,3→2,4→3，
// 其它保持 0)；handleStartGame 失败时 _destroy()；返回其结果。
// ----------------------------------------------------------------------------
bool Global::CBossTowerFunc::SelectDungeon(CParty* party, short dungeonIdx,
                                           ENUM_DUNGEON_TYPE type)
{
    if (party == 0)
    {
        return false;
    }

    WongWork::CBossTower* tower = G_CGameManager()->getBossTower();
    if (tower == 0)
    {
        cMyTrace trace(
            "static bool Global::CBossTowerFunc::SelectDungeon(CParty*, short int, ENUM_DUNGEON_TYPE)",
            0x34, 5);
        trace("StaticPool BossTower NULL");
        return false;
    }

    char memberCount = 0;
    switch (party->get_member_count())
    {
        case 2: memberCount = 1; break;
        case 3: memberCount = 2; break;
        case 4: memberCount = 3; break;
        default: break;  // ORIG: count==1 或越界值保持 0
    }

    const bool started = tower->handleStartGame(party, static_cast<int>(dungeonIdx),
                                                memberCount, type);
    if (!started)
    {
        tower->_destroy();
    }
    return started;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08655220 _ZN5CUser12getBossTowerEv
// m_bossTowerIdx(+0x8d010 short)<0 → 0；否则 G_CGameManager()->getBossTower(idx)。
// ----------------------------------------------------------------------------
WongWork::CBossTower* CUser::getBossTower()
{
    const short idx = m_bossTowerIdx;
    if (idx < 0)
    {
        return 0;
    }
    return G_CGameManager()->getBossTower(static_cast<int>(idx));
}

// ----------------------------------------------------------------------------
// ORIG T 0x08655262 _ZN5CUser15getAdvanceAltarEv
// m_advanceAltarIdx(+0x8d012 short)<0 → 0；否则 G_CGameManager()->getAdvanceAltar(idx)。
// ----------------------------------------------------------------------------
advancealtar::StageControl* CUser::getAdvanceAltar()
{
    const short idx = static_cast<short>(m_advanceAltarIdx);
    if (idx < 0)
    {
        return 0;
    }
    return G_CGameManager()->getAdvanceAltar(static_cast<int>(idx));
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146824 _ZN6Global14CBossTowerFunc18DieCharacInDungeonEP5CUser
// state==10(死亡塔) → CDeathTower::handleDieUser；state==12(BossTower) → 虚槽[4]
// handleDieUser；其它 state 返回 0。两分支均返回 1。
// ----------------------------------------------------------------------------
char Global::CBossTowerFunc::DieCharacInDungeon(CUser* user)
{
    const int state = user->get_state();
    if (state == 0xa)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            tower->handleDieUser(user);
        }
        return 1;
    }
    if (state == 0xc)
    {
        WongWork::CBossTower* tower = user->getBossTower();
        if (tower != 0)
        {
            tower->handleDieUser(user);  // ORIG vptr+0x10 = 槽[4] handleDieUser
        }
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146916 _ZN6Global14CBossTowerFunc10FinishLoadEP5CUser
// state==10 → CDeathTower::handleFinishLoading；state==12 →
// CBossTower::handleFinishLoading（直调）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
char Global::CBossTowerFunc::FinishLoad(CUser* user)
{
    const int state = user->get_state();
    if (state == 0xa)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            tower->handleFinishLoading(user);
        }
        return 1;
    }
    if (state == 0xc)
    {
        WongWork::CBossTower* tower = user->getBossTower();
        if (tower != 0)
        {
            tower->handleFinishLoading(user);  // ORIG 直调 0x08143f6a（非虚分派）
        }
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146ad8 _ZN6Global14CBossTowerFunc10GiveUpGameEP5CUser
// state==10 → CDeathTower::handleLeaveUser；state==12 → 虚槽[5] handleLeaveUser；
// state==5 且队伍存在 → giveup_game(user,true,false,false)；恒返回 1。
// ----------------------------------------------------------------------------
void Global::CBossTowerFunc::GiveUpGame(CUser* user)
{
    const int state = user->get_state();
    if (state == 0xa)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            tower->handleLeaveUser(user);
        }
    }
    else if (state == 0xc)
    {
        WongWork::CBossTower* tower = user->getBossTower();
        if (tower != 0)
        {
            tower->handleLeaveUser(user);  // ORIG vptr+0x14 = 槽[5] handleLeaveUser
        }
    }
    else if (state == 5)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            party->giveup_game(user, true, false, false);
        }
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146ba2 _ZN6Global14CBossTowerFunc7MoveMapEP5CUserPt
// state==10 → CDeathTower::handleMoveMap()；state==12 → 虚槽[6] handleMoveMap(
// mapIndex)（CBossStage 新增虚）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
char Global::CBossTowerFunc::MoveMap(CUser* user, unsigned short* mapIndex)
{
    const int state = user->get_state();
    if (state == 0xa)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            tower->handleMoveMap();
        }
        return 1;
    }
    if (state == 0xc)
    {
        WongWork::CBossTower* tower = user->getBossTower();
        if (tower != 0)
        {
            tower->handleMoveMap(mapIndex);  // ORIG vptr+0x18 = 槽[6] handleMoveMap
        }
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081466f0 _ZN6Global11CGlobalFunc15Get_State_CheckE8ch_state
// （docs/class_func_reports/Global__CGlobalFunc/Get_State_Check.md ## 汇编 +
// ## 反编译 C 全文转写）：cmp $0xd + ja（无符号比较）→ 越界直接 0；否则
// (1 << state) & 0x3420 测试位 5/6/12/13（5=队伍玩法、12(0xc)=BossTower，
// 参见同文件 UseItem/DieCharacInDungeon 的 state 分支注释）。命中 → 1，否则 0。
// ORIG 调用方（objdump xref）：DisPatcher_FinishLoad::check_state(0x82654a6)、
// 0x81c2bd4、0x81c4b5b、0x81feef2、0x8232256、0x82324c0。
bool Global::CGlobalFunc::Get_State_Check(ch_state state)
{
    if (static_cast<unsigned int>(state) > 0xd)
    {
        return false;
    }

    unsigned int bit = 1;
    bit <<= static_cast<unsigned int>(state);
    if ((bit & 0x3420u) != 0)
    {
        return true;
    }
    return false;
}
