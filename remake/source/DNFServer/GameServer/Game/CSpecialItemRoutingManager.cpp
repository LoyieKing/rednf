// CSpecialItemRoutingManager.cpp
// ORIG 依据：docs/class_func_reports/CSpecialItemRoutingManager/StartRouting.md
//   + objdump 0x0860a9d4/0x0860b45c/0x0860b4c2 逐指令忠实转写。
#include "CSpecialItemRoutingManager.h"

#include "CDataManager.h"  // G_CDataManager / find_item
#include "CParty.h"        // IsRoutingState/SetStartRouting/SetRoutingItem/
                           // gen_timer_key/GetPartyIndex/send_to_party
#include "CUser.h"         // get_state/GetParty
#include "Inven_Item.h"    // m_addInfo（+0x02 item idx）
#include "PacketGuard.h"
#include "TimerQueue.h"    // G_TimerQueue/TimerEntry/TIMER_MESSAGE

// ORIG 0x0860a9d4 _ZN26CSpecialItemRoutingManager12StartRoutingEP5CUserR10Inven_Itemj
// get_state∉{5,0xc}→1；GetParty()==0→1；find_item(item idx)==0→1；
// IsRoutingState()→0x12；否则置路由状态/物品、insertTimer(0x23s)、
// put_header(0,0x119)+put_int(item idx)+finalize(true) 后 send_to_party，返回 0。
int CSpecialItemRoutingManager::StartRouting(CUser* user, Inven_Item& item,
                                             unsigned int uniqueId)
{
    int state = user->get_state();
    if (state != 5 && state != 0xc)
        return 1;

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return 1;

    CDataManager* dataMgr = G_CDataManager();
    if (dataMgr->find_item(item.m_addInfo) == 0)
        return 1;

    if (party->IsRoutingState())
        return 0x12;

    party->SetStartRouting();
    party->SetRoutingItem(item, uniqueId);
    insertTimer(party);

    PacketGuard packet;
    packet.put_header(0, 0x119);
    packet.put_int(item.m_addInfo);
    packet.finalize(true);
    party->send_to_party(packet);
    return 0;
}

// ORIG 0x0860b45c _ZN26CSpecialItemRoutingManager11insertTimerEP6CParty
// key=gen_timer_key(0x2b)；InsertTimer(OBJ_TYPE 1, GetPartyIndex(),
// msg 0x2b, sec 0x23, key, 0)。
void CSpecialItemRoutingManager::insertTimer(CParty* party)
{
    int timerKey = party->gen_timer_key((TIMER_MESSAGE)0x2b);
    int partyIndex = party->GetPartyIndex();
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, partyIndex,
                                (TIMER_MESSAGE)0x2b, 0x23, timerKey, 0);
}

// ORIG 0x0860b4c2 _ZN26CSpecialItemRoutingManager11cancelTimerEP6CParty
int CSpecialItemRoutingManager::cancelTimer(CParty* party)
{
    return party->gen_timer_key((TIMER_MESSAGE)0x2b);
}

// ORIG T 0x0860b2c2（报告 CSpecialItemRoutingManager/ProcessGiveUp.md）：
// party 非空时——路由未开启 或 全员完成选卡（HasAllMemberSelection）——
// 且首个有效成员状态为 5（短路返回）或再次读取为 0xc 时，执行 processRouting。
void CSpecialItemRoutingManager::ProcessGiveUp(CParty* party)
{
    if (party == 0)
        return;

    bool routable;
    if (!party->IsRoutingState() || !party->HasAllMemberSelection())
    {
        routable = false;
    }
    else
    {
        char state = party->GetFirstValidMemberState();
        if (state == 5)
            routable = true;
        else
            routable = (party->GetFirstValidMemberState() == 0xc);
    }

    if (routable)
        processRouting(party);
}

CSpecialItemRoutingManager::CSpecialItemRoutingManager()
{
    // ORIG 0x0860a9c8：空函数（push ebp; mov; pop ebp; ret），不初始化任何成员。
}
