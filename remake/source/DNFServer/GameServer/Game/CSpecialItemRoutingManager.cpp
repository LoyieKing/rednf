// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSpecialItemRoutingManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "CSpecialItemRoutingManager/CSpecialItemRoutingManager.inc"
{
    // ORIG 0x0860a9c8：空函数（push ebp; mov; pop ebp; ret），不初始化任何成员。
}

// [b28d-64 判词汇总 · 泳道64] 本 TU 1 件：StartRouting 0x0860a9d4
//   102/102 N封顶（P251 候补：ORIG `cmp $0x5,%eax; cmp $0xc,%eax` 寄存器直比 +
//   两 nop 岛 vs 我方具名 state 落槽 -0x14 无岛；语义等价 ⇒ N82/N7）。
// [M5A_DispLink] 102/104：ORIG 0x0860aa25 `mov %eax,%ebx`（item idx 驻留跨 call）+
//   0x0860aa36 `mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp)`（具名 CItem* 槽直比）⇒ 旧形
//   （具名 dataMgr + 返回值直接 `== 0`）多 2 条（sete 链 + 求值顺序倒置）；改链式调用。

// ORIG 0x0860a9d4 _ZN26CSpecialItemRoutingManager12StartRoutingEP5CUserR10Inven_Itemj
// get_state∉{5,0xc}→1；GetParty()==0→1；find_item(item idx)==0→1；
// IsRoutingState()→0x12；否则置路由状态/物品、insertTimer(0x23s)、
// put_header(0,0x119)+put_int(item idx)+finalize(true) 后 send_to_party，返回 0。
#include "CSpecialItemRoutingManager/StartRouting.inc"
                                             unsigned int uniqueId)
{
    // [round3-L30 修] ORIG 0x0860a9d4：`call get_state; cmp $0x5,%eax; je; cmp $0xc,%eax;
    //   je`（**一次** call 结果留 %eax 复用、无栈槽）⇒ 按 P261/P279「无状态槽 + cmp $K,%eax
    //   链 ⇒ 源形是 switch(call())」改写；旧形 `int state = ...; if (state != 5 && state != 0xc)`
    //   引入 ORIG 无的具名槽且第二次比较退化为槽直比。
    switch (user->get_state())
    {
    case 5:
    case 0xc:
        break;
    default:
        return 1;
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return 1;

    // ORIG 0x0860aa1d-0x0860aa36：`mov 0x10(%ebp),%eax; mov 0x2(%eax),%eax; mov %eax,%ebx;
    //   call G_CDataManager; mov %ebx,0x4(%esp); mov %eax,(%esp); call find_item;
    //   mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp); jne` ⇒ 源码为链式
    //   `G_CDataManager()->find_item(item.m_addInfo)`（实参先求值、this 后求值），
    //   结果落具名 CItem* 槽 -0xc。
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
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
// b28c-L20: AE — P0 清绑（原 register asm("esi"/"ebx") 违反全域禁令）；P24 内联
// 实参自然形复现 ORIG 的 key→%esi / index→%ebx / G_TimerQueue() 置后三件套
// （ndiff 清绑前 >0 → 0，gap=0，N96 清绑正向例）。
#include "CSpecialItemRoutingManager/insertTimer.inc"
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x2b, 0x23,
                                party->gen_timer_key((TIMER_MESSAGE)0x2b), 0);
}

// ORIG 0x0860b4c2 _ZN26CSpecialItemRoutingManager11cancelTimerEP6CParty
#include "CSpecialItemRoutingManager/cancelTimer.inc"
{
    return party->gen_timer_key((TIMER_MESSAGE)0x2b);
}

// ORIG T 0x0860b2c2（报告 CSpecialItemRoutingManager/ProcessGiveUp.md）：
// party 非空时——路由未开启 或 全员完成选卡（HasAllMemberSelection）——
// 且首个有效成员状态为 5（短路返回）或再次读取为 0xc 时，执行 processRouting。
#include "CSpecialItemRoutingManager/ProcessGiveUp.inc"
{
    if (party == 0)
        return;

    if (party->IsRoutingState() && party->HasAllMemberSelection()
        && (party->GetFirstValidMemberState() == 5
            || party->GetFirstValidMemberState() == 0xc))
        processRouting(party);
}

#include "CSpecialItemRoutingManager/dtor_CSpecialItemRoutingManager.inc"
{
    // ORIG T 0x0860a9ce：空函数（push ebp; mov; pop ebp; ret）。
}

// ORIG 0x0860abde（报告 CSpecialItemRoutingManager/ProcessTimeOut.md）：
// IsRoutingState()→GetFirstValidMemberState()==5/0xc 时 SetRoutingTimerOver+
// HasAllMemberSelection→processRouting；否则 SetEndRouting。恒返回 1。
// b28c-L20: P0 清绑（原 register int state asm("eax") 违反全域禁令）——ORIG 对
// GetFirstValidMemberState 只发一次 call 且结果无栈槽往返（movsbl %al,%eax;
// cmp $5,%eax; je; cmp $0xc,%eax; je + 两处空 case nop 岛），即原码为
// switch(expr) 双 case 共享块，非具名局部。
// b28c-L20: 清绑后 switch(default 前置)形复现决策树与单次 call。
// [b28d-58] DIFF N（P223 族，空 case 岛）：ORIG 0x0860abde 两个 case 块为
//   4.1 空 case 布局（case5 = `nop; jmp +0x58`、case0xc = `nop` 直落 body）；
//   4.4 对 switch 空 case 恒 jump-thread 成双 `je body`（实测三形：独立
//   break / 共享 goto 标签 / 双 goto 串——岛数恒 0）⇒ 2 nop+1 jmp 封顶，
//   语义等价（gap=3）。
#include "CSpecialItemRoutingManager/ProcessTimeOut.inc"
{
    if (!party->IsRoutingState())
    {
        party->SetEndRouting();
        return 1;
    }
    switch (party->GetFirstValidMemberState())
    {
    default:
        party->SetEndRouting();
        return 1;
    case 5:
        break;
    case 0xc:
        break;
    }
    party->SetRoutingTimerOver();
    if (party->HasAllMemberSelection())
        processRouting(party);
    return 1;
}
// ORIG T 0x0860ab2e（报告 CSpecialItemRoutingManager/ArriveMemberSelection.md
// 全文转写）：GetMemberRoutingState(user) 已非 0 → 返回 0（已选过）；
// 否则 sel==0 → SetMemberRoutingState(user,1)、sel==1 → SetMemberRoutingState(user,2)
// （其它值不写状态）；随后 HasAllMemberSelection() 则 cancelTimer+processRouting；
// 返回 1。
// b28c-L20: AE — 修「恒返回 0」真语义 bug（原 goto FAIL 让成功路径也落到 return 0）；
// [b28d-58] AE：FAIL 块（return 0，ORIG +0x6e）夹在 sel 链与 HasAll 之间，
// CHECK 在 +0x75 —— 该块序即 `if (state == 0) { sel 链 } else { return 0; }`
// 的 4.1 布局；原 goto FAIL/CHECK 形令 4.4 在 FAIL 前后各插一条跳转对齐 nop
// （+0x6e/+0x76）且 CHECK 漂到 +0x77 ⇒ 改自然 if/else 后逐条全等。
#include "CSpecialItemRoutingManager/ArriveMemberSelection.inc"
                                                      char sel)
{
    int state = party->GetMemberRoutingState(user);           // ORIG 0x085ba150
    if (state == 0)
    {
        if (sel == 0)
            party->SetMemberRoutingState(user, 1);            // ORIG 0x085ba0d8
        else if (sel == 1)
            party->SetMemberRoutingState(user, 2);
    }
    else
    {
        return 0;
    }
    if (party->HasAllMemberSelection())                       // ORIG 0x085ba1c0
    {
        cancelTimer(party);
        processRouting(party);
    }
    return 1;
}
