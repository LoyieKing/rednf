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
    ~CSpecialItemRoutingManager();  // ORIG T 0x0860a9ce（空体，显式定义）

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
    int ProcessTimeOut(CParty* party);   // ORIG T 0x0860abde（Timer_RoutingItemWait 调用）
    // ORIG T 0x0860ab2e（报告 CSpecialItemRoutingManager/ArriveMemberSelection.md）：
    // 成员选卡到达。GetMemberRoutingState==0 时按 sel(0→1,1→2) SetMemberRoutingState，
    // HasAllMemberSelection 则 cancelTimer+processRouting；返回 1=处理 0=已选过。
    // [R16a13 · P292/P168a①] ORIG 全二进制唯一调用点 0x08256ba（Dispatcher_
    // BiddingRoutingItem::dispatch_sig）形为 `call; xor $0x1,%eax; test %al,%al; je`
    // ⇒ callee 返回类型 bool（int 形 4.4 恒产 `test;sete` 两段，且调用点无法自救）。
    // 定义侧 0x0860ab2e 尾 `mov $0x1/%0x0,%eax` 与 bool 常量返回逐字节相同（探针
    // /tmp/verify357/R16a13/probe_bool.s：int/bool 的 `return 1;` 均为 `movl $1,%eax`）
    // ⇒ 改型零 mangled/布局影响（返回类型不入 mangled；空类 sizeof=1 不变）。
    bool ArriveMemberSelection(CUser* user, CParty* party, char sel);

// ORIG 空类（sizeof 1，GetSpecialItemRoutingManager _Znwj(0x1) 实证；
// 各方法经 CParty/全局数据工作，不访问 this 成员）。
};

#endif  // GAME_CSpecialItemRoutingManager_H