// CSpecialItemRoutingManager.h
// Reconstruction of CSpecialItemRoutingManager (ORIG _ZN26CSpecialItemRoutingManagerC1Ev).
#ifndef GAME_CSpecialItemRoutingManager_H
#define GAME_CSpecialItemRoutingManager_H
class CUser;
class Inven_Item;
class CParty;

class CSpecialItemRoutingManager
{
public:
    CSpecialItemRoutingManager();

    // ORIG T 0x0860a9d4：状态/队伍/物品校验 → SetStartRouting/SetRoutingItem
    // → insertTimer → 广播 put_header(0,0x119)+put_int(item idx)。
    // 返回：1=条件不满足，0x12=已在路由中，0=成功。
    int StartRouting(CUser* user, Inven_Item& item, unsigned int uniqueId);

    // ORIG T 0x0860b45c：gen_timer_key(0x2b) + TimerQueue::InsertTimer
    //   (OBJ_TYPE 1, partyIndex, msg 0x2b, sec 0x23, key, 0)。
    void insertTimer(CParty* party);

    // ORIG T 0x0860b4c2：返回 gen_timer_key(0x2b)。
    int cancelTimer(CParty* party);

    // ORIG T 0x0860b332（blocked：定义待补；DisPatcher_MoveMap::process 在
    // CParty::IsRoutingState() 为真且 mgr 非空时以 (party, 0) 调用）
    void ProcessFailEtc(CParty* party, int param);
    // ORIG T 0x0860b2c2（报告 CSpecialItemRoutingManager/ProcessGiveUp.md）：
    // 放弃副本收尾——路由未开启或全员选卡完成、且首个有效成员状态∈{5,0xc}
    // 时调用 processRouting。CParty::giveup_game 结尾调用。
    void ProcessGiveUp(CParty* party);

    // ORIG T 0x0860ac6a（blocked：定义待补；ProcessGiveUp 命中条件时调用）
    void processRouting(CParty* party);

private:
    char m_pad[0x8];
};

#endif  // GAME_CSpecialItemRoutingManager_H