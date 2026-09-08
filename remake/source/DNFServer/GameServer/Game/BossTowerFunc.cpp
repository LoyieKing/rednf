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
    switch (user->get_state())
    {
    case 5:
        {
            // ORIG _ZN5CUser8GetPartyEv 返回 CParty*；现头声明为 void*，此处语义转换
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->useItem(user, item);
            }
        }
        return true;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleUseItem(user,
                                     static_cast<unsigned long>(item.m_addInfo),  // +0x02 dword
                                     item.m_field1);                              // +0x01 ITEM_TYPE byte
            }
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146994 _ZN6Global14CBossTowerFunc7GetItemEP5CUseribc
// getBossTower() 判空 → cMyTrace("bossTower not find : charac_no - %u") 恒返 1；
// 否则虚槽[10] handlePickupItem(user,item,flag,type)；非零 →
// PacketGuard: put_header(1,0x2e)+put_byte(1)+finalize(true)+user->Send。
// ----------------------------------------------------------------------------
char Global::CBossTowerFunc::GetItem(CUser* user, int item, bool flag, char type)
{
    WongWork::CBossTower* tower = user->getBossTower();
    if (tower != 0)
    {
        const int result = tower->handlePickupItem(user, item, flag, type);
        if (result != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0x2e);
            packet.put_byte(1);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    else
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xb7, 5);
        trace("bossTower not find : charac_no - %u", user->getCurCharacNo());
    }
    return 1;
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
            __PRETTY_FUNCTION__,
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
    // ORIG 0x08655220：无 idx 槽——movzwl 直读→test %ax,%ax; js→尾零块+
    // call 后 jmp 中继 = 三目返回形（P47）；传参点重读 movzwl+movswl（P200）。
    return ((short)m_bossTowerIdx < 0)
               ? 0
               : G_CGameManager()->getBossTower(
                     static_cast<int>(static_cast<short>(m_bossTowerIdx)));
}

// ----------------------------------------------------------------------------
// ORIG T 0x08655262 _ZN5CUser15getAdvanceAltarEv
// m_advanceAltarIdx(+0x8d012 short)<0 → 0；否则 G_CGameManager()->getAdvanceAltar(idx)。
// ----------------------------------------------------------------------------
advancealtar::StageControl* CUser::getAdvanceAltar()
{
    // ORIG 0x08655262：同 getBossTower——三目返回形。
    return ((short)m_advanceAltarIdx < 0)
               ? 0
               : G_CGameManager()->getAdvanceAltar(
                     static_cast<int>(static_cast<short>(m_advanceAltarIdx)));
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146824 _ZN6Global14CBossTowerFunc18DieCharacInDungeonEP5CUser
// state==10(死亡塔) → CDeathTower::handleDieUser；state==12(BossTower) → 虚槽[4]
// handleDieUser；其它 state 返回 0。两分支均返回 1。
// b28c-L8 判词 [N(N7)]：原 if 链 + 具名 `const int state` 已改
// switch(user->get_state()) —— ORIG 0x08146824 无 state 栈槽、`cmp $0xa,%eax;je /
// cmp $0xc,%eax;je / jmp` 决策链 = P0「值无槽往返⇒右值内联」+ P17 switch 形（if 链下
// 4.4 必产 store+mem 比较）；翻形后 44=44 逐条等。残 7 行 N7：ORIG
// `mov (%eax),%edx … call *%edx`（vptr+0x10 handleDieUser），4.4 对间接 call 目标恒
// %eax、其余装载寄存器随之整体后移一格（助记符/长度全等，仅寄存器名差）。
// ----------------------------------------------------------------------------
char Global::CBossTowerFunc::DieCharacInDungeon(CUser* user)
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);  // ORIG vptr+0x10 = 槽[4] handleDieUser
            }
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
bool Global::CBossTowerFunc::FinishLoad(CUser* user)  // ORIG T 0x08146916
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleFinishLoading(user);
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleFinishLoading(user);  // ORIG 直调 0x08143f6a（非虚分派）
            }
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
// b28c-L8 判词 [N(N7)]：同 DieCharacInDungeon 的 switch 翻形（ORIG 0x08146ba2 42=42
// 逐条等）；残 6 行 = 虚槽 +0x18 handleMoveMap 的间接 call 目标 %edx vs 4.4 恒 %eax。
bool Global::CBossTowerFunc::MoveMap(CUser* user, unsigned short* mapIndex)  // ORIG T 0x08146ba2
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleMoveMap();
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleMoveMap(mapIndex);  // ORIG vptr+0x18 = 槽[6] handleMoveMap
            }
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
    // b28c-L8 复测（N6 位折叠判据）：ORIG 0x081466f0 保 `mov $0x1,%edx;
    // mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx; mov %ebx,%eax; and $0x3420`
    // 的**左移位图**形。4.4 前端会把 `(1 << state) & 0x3420` 折成
    // `0x3420 >> state` 的 shr 形（N6），唯一阻断法是让被移位值成为占槽局部
    // （实测具名 bit 形保 shl，但引入 store/load）⇒ 封顶 N6。
    // 本形同时按 P222 取正向包裹，使越界路径直落尾部共享 `mov $0` 岛
    // （ORIG 两处失败均 `ja/je` 跳同一 +0x29 岛）。
    if (static_cast<unsigned int>(state) <= 0xd)
    {
        unsigned int bit = 1;
        bit <<= static_cast<unsigned int>(state);
        if ((bit & 0x3420u) != 0)
        {
            return true;
        }
    }
    return false;
}
