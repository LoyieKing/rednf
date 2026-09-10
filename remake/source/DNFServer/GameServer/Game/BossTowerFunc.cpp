// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BossTowerFunc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
// ORIG T 0x081466f0 _ZN6Global11CGlobalFunc15Get_State_CheckE8ch_state
// （docs/class_func_reports/Global__CGlobalFunc/Get_State_Check.md ## 汇编 +
// ## 反编译 C 全文转写）：cmp $0xd + ja（无符号比较）→ 越界直接 0；否则
// (1 << state) & 0x3420 测试位 5/6/12/13（5=队伍玩法、12(0xc)=BossTower，
// 参见同文件 UseItem/DieCharacInDungeon 的 state 分支注释）。命中 → 1，否则 0。
// ORIG 调用方（objdump xref）：DisPatcher_FinishLoad::check_state(0x82654a6)、
// 0x81c2bd4、0x81c4b5b、0x81feef2、0x8232256、0x82324c0。
// [b28d-D9] N18a：P231 裸 register 消槽成功（20/20）；残 2 条 = 4.1 冗余 mov $1,%edx; mov %edx,%ebx 不可造（ORIG 0x081466f0 sz 0x00000031）
#include "BossTowerFunc/Get_State_Check.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146c20 _ZN6Global14CBossTowerFunc7UseItemEP5CUserRK10Inven_Item
// state==5(队伍玩法) → CParty::useItem；state==12(0xc,BossTower) →
// CBossTower::handleUseItem(item.m_addInfo(+0x02), item.m_field1(+0x01))。
// 两分支均返回 1；其它 state 返回 0。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/UseItem.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146994 _ZN6Global14CBossTowerFunc7GetItemEP5CUseribc
// getBossTower() 判空 → cMyTrace("bossTower not find : charac_no - %u") 恒返 1；
// 否则虚槽[10] handlePickupItem(user,item,flag,type)；非零 →
// PacketGuard: put_header(1,0x2e)+put_byte(1)+finalize(true)+user->Send。
// ----------------------------------------------------------------------------
// [b28d-D9] P227/N82：cMyTrace 临时链 %ebx 保活 + PacketGuard 槽位差（ORIG 0x08146994 sz 0x00000144）
#include "BossTowerFunc/GetItem.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146722 _ZN6Global14CBossTowerFunc13SelectDungeonEP6CPartys17ENUM_DUNGEON_TYPE
// party 判空 → 0；getBossTower() 判空 → cMyTrace(PRETTY_FUNCTION,0x34,5) +
// "StaticPool BossTower NULL" → 0；按 get_member_count 映射 (1→0,2→1,3→2,4→3，
// 其它保持 0)；handleStartGame 失败时 _destroy()；返回其结果。
// ----------------------------------------------------------------------------
// [b28d-D9] N82/槽差：帧/槽序差（74 vs 66）（ORIG 0x08146722 sz 0x00000102）
#include "BossTowerFunc/SelectDungeon.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08655220 _ZN5CUser12getBossTowerEv
// m_bossTowerIdx(+0x8d010 short)<0 → 0；否则 G_CGameManager()->getBossTower(idx)。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/getBossTower.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08655262 _ZN5CUser15getAdvanceAltarEv
// m_advanceAltarIdx(+0x8d012 short)<0 → 0；否则 G_CGameManager()->getAdvanceAltar(idx)。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/getAdvanceAltar.inc"
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
#include "BossTowerFunc/DieCharacInDungeon.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146916 _ZN6Global14CBossTowerFunc10FinishLoadEP5CUser
// state==10 → CDeathTower::handleFinishLoading；state==12 →
// CBossTower::handleFinishLoading（直调）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
#include "BossTowerFunc/FinishLoad.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146ad8 _ZN6Global14CBossTowerFunc10GiveUpGameEP5CUser
// state==10 → CDeathTower::handleLeaveUser；state==12 → 虚槽[5] handleLeaveUser；
// state==5 且队伍存在 → giveup_game(user,true,false,false)；恒返回 1。
// ----------------------------------------------------------------------------
// [b28d-D9] N82/槽差：帧/槽序差（59 vs 57）（ORIG 0x08146ad8 sz 0x000000ca）
// ORIG 尾 `mov $0x1,%eax; leave; ret`（单出口返 1）；case 体依源码序 5/0xa/0xc
// 落槽 -0x14/-0x10/-0xc（ORIG 深→浅 = 4.1「先声明更深」）。
#include "BossTowerFunc/GiveUpGame.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08146ba2 _ZN6Global14CBossTowerFunc7MoveMapEP5CUserPt
// state==10 → CDeathTower::handleMoveMap()；state==12 → 虚槽[6] handleMoveMap(
// mapIndex)（CBossStage 新增虚）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
// b28c-L8 判词 [N(N7)]：同 DieCharacInDungeon 的 switch 翻形（ORIG 0x08146ba2 42=42
// 逐条等）；残 6 行 = 虚槽 +0x18 handleMoveMap 的间接 call 目标 %edx vs 4.4 恒 %eax。
#include "BossTowerFunc/MoveMap.inc"
}
