// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSpecialItemRoutingManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

// [b28d-64 判词汇总 · 泳道64] 本 TU 1 件：StartRouting 0x0860a9d4
//   102/102 N封顶（P251 候补：ORIG `cmp $0x5,%eax; cmp $0xc,%eax` 寄存器直比 +
//   两 nop 岛 vs 我方具名 state 落槽 -0x14 无岛；语义等价 ⇒ N82/N7）。

// ORIG 0x0860a9d4 _ZN26CSpecialItemRoutingManager12StartRoutingEP5CUserR10Inven_Itemj
// get_state∉{5,0xc}→1；GetParty()==0→1；find_item(item idx)==0→1；
// IsRoutingState()→0x12；否则置路由状态/物品、insertTimer(0x23s)、
// put_header(0,0x119)+put_int(item idx)+finalize(true) 后 send_to_party，返回 0。
#include "CSpecialItemRoutingManager/StartRouting.inc"
}

// ORIG 0x0860b45c _ZN26CSpecialItemRoutingManager11insertTimerEP6CParty
// key=gen_timer_key(0x2b)；InsertTimer(OBJ_TYPE 1, GetPartyIndex(),
// msg 0x2b, sec 0x23, key, 0)。
// b28c-L20: AE — P0 清绑（原 register asm("esi"/"ebx") 违反全域禁令）；P24 内联
// 实参自然形复现 ORIG 的 key→%esi / index→%ebx / G_TimerQueue() 置后三件套
// （ndiff 清绑前 >0 → 0，gap=0，N96 清绑正向例）。
#include "CSpecialItemRoutingManager/insertTimer.inc"
}

// ORIG 0x0860b4c2 _ZN26CSpecialItemRoutingManager11cancelTimerEP6CParty
#include "CSpecialItemRoutingManager/cancelTimer.inc"
}

// ORIG T 0x0860b2c2（报告 CSpecialItemRoutingManager/ProcessGiveUp.md）：
// party 非空时——路由未开启 或 全员完成选卡（HasAllMemberSelection）——
// 且首个有效成员状态为 5（短路返回）或再次读取为 0xc 时，执行 processRouting。
#include "CSpecialItemRoutingManager/ProcessGiveUp.inc"
}

#include "CSpecialItemRoutingManager/dtor_CSpecialItemRoutingManager.inc"
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
}
