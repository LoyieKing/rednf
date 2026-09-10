// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CHackLog_InvalidAreaMove/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN24CHackLog_InvalidAreaMove11GetHackTypeEv`（ORIG 0x828619a）；
// GetHackType 定义已是本 TU 首强符号。
#include <iostream>

#include "CHackLog_InvalidAreaMove.h"

// 全部方法按 ORIG df_game_r（elf32-i386）逐指令反汇编转写。
// 反汇编命令：objdump -d /home/wangyilei/下载/df_game_r
// 地址与符号见各方法注释。

// ORIG 0x08285e14
//   _ZN24CHackLog_InvalidAreaMove14checkSeriaRoomEii
//   直接返回 IsSeriaRoom(a, b) 的结果（eax 保留）。
// [A31-GI · P327] 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN24CHackLog_InvalidAreaMove14checkSeriaRoomEii`（ORIG 0x828619a）。
#include "CHackLog_InvalidAreaMove/checkSeriaRoom.inc"
}

// ORIG 0x086d272c
//   _ZN24CHackLog_InvalidAreaMove11GetHackTypeEv
//   86d272c: mov $0x1f5,%eax
#include "CHackLog_InvalidAreaMove/GetHackType.inc"
}

// ORIG 0x082861b6
//   _ZN24CHackLog_InvalidAreaMove31IsMovingBetweenWestCoastAndGentEiiii
//   if (IsGentEntranceInWestCoast(a,b) && IsWestCostEntranceInGent(c,d)) return 1;
//   if (IsWestCostEntranceInGent(a,b) && IsGentEntranceInWestCoast(c,d)) return 1;
//   return 0;
#include "CHackLog_InvalidAreaMove/IsMovingBetweenWestCoastAndGent.inc"
}

// ORIG 0x082862d0
//   _ZN24CHackLog_InvalidAreaMove25IsGentEntranceInWestCoastEii
//   a == 3 && b == 6
#include "CHackLog_InvalidAreaMove/IsGentEntranceInWestCoast.inc"
}

// ORIG 0x082862ee
//   _ZN24CHackLog_InvalidAreaMove24IsWestCostEntranceInGentEii
//   a == 6 && b == 0
#include "CHackLog_InvalidAreaMove/IsWestCostEntranceInGent.inc"
}

// ORIG 0x08286222
//   _ZN24CHackLog_InvalidAreaMove21IsMovingAboutPowerWarEii
//   if (IsHendonMyre(a) && IsWarfare(b)) return 1;
//   if (IsWarfare(a) && IsHendonMyre(b)) return 1;
//   return 0;
#include "CHackLog_InvalidAreaMove/IsMovingAboutPowerWar.inc"
}

// ORIG 0x08286330
//   _ZN24CHackLog_InvalidAreaMove19IsMovingInGuildAgitEii
//   a == 8 && a == b
#include "CHackLog_InvalidAreaMove/IsMovingInGuildAgit.inc"
}

// ORIG 0x08286350
//   _ZN24CHackLog_InvalidAreaMove15IsExceptionMoveEiiii
//   任一四元组匹配即返回 1：
//   (6,0,6,4) (6,4,6,0) (3,8,3,0) (3,0,3,8) (3,1,2,0)
//   (2,0,3,1) (2,2,2,5) (2,5,2,2) (2,1,2,5) (2,5,2,1)
#include "CHackLog_InvalidAreaMove/IsExceptionMove.inc"
}

// ORIG 0x08286272
//   _ZN24CHackLog_InvalidAreaMove36IsMovingBetweenSeriaRoomAndGuildAgitEiiii
//   if (IsSeriaRoom(a,b) && IsGuildAgit(c)) return 1;
//   if (IsGuildAgit(a) && IsSeriaRoom(c,d)) return 1;
//   return 0;
#include "CHackLog_InvalidAreaMove/IsMovingBetweenSeriaRoomAndGuildAgit.inc"
}


// ORIG 0x08285e2e
//   _ZN24CHackLog_InvalidAreaMove20IsInvalidMovableAreaEiiii
//   任一谓词为真 -> 0；全部为假 -> 1。
//   谓词依次：
//     IsMovingBetweenWestCoastAndGent(a,b,c,d)
//     IsMovingAboutPowerWar(a,c)
//     IsMovingInGuildAgit(a,c)
//     IsExceptionMove(a,b,c,d)
//     IsMovingBetweenSeriaRoomAndGuildAgit(a,b,c,d)
#include "CHackLog_InvalidAreaMove/IsInvalidMovableArea.inc"
}

// ORIG 0x08285efe
//   _ZN24CHackLog_InvalidAreaMove14isMovableRouteEiiiiii
//   任一六元组匹配即返回 1，否则 0（局部 char 初值 0，命中置 1）：
//   (3,2,3,0,2,0) (3,1,3,0,2,0) (2,0,3,0,3,1) (2,0,3,0,3,2)
//   (6,0,3,6,3,1) (4,1,4,0,4,3) (4,3,4,0,4,1) (1,3,4,0,4,4)
//   (4,4,4,0,1,3) (2,2,2,8,5,0) (5,0,2,8,2,2) (2,4,2,0,3,0)
//   (4,1,4,0,1,3) (4,0,1,3,1,0) (3,6,6,0,6,1)
#include "CHackLog_InvalidAreaMove/isMovableRoute.inc"
}

// ORIG 0x0828630c
//   _ZN24CHackLog_InvalidAreaMove12IsHendonMyreEi
//   a == 2
#include "CHackLog_InvalidAreaMove/IsHendonMyre.inc"
}

// ORIG 0x08286318
//   _ZN24CHackLog_InvalidAreaMove9IsWarfareEi
//   a == 7
#include "CHackLog_InvalidAreaMove/IsWarfare.inc"
}

// ORIG 0x08286324
//   _ZN24CHackLog_InvalidAreaMove11IsGuildAgitEi
//   a == 8
#include "CHackLog_InvalidAreaMove/IsGuildAgit.inc"
}

// ORIG 0x08286462
//   _ZN24CHackLog_InvalidAreaMove11IsSeriaRoomEii
//   任一 (a,b) 匹配即返回 1：
//   (1,1) (2,5) (3,2) (4,1) (5,1) (6,4) (9,2) (11,3)
#include "CHackLog_InvalidAreaMove/IsSeriaRoom.inc"
}

// ORIG 0x086c4568
//   _Z42Is_restrictive_commercial_transaction_zoneiiii
//   自由函数；点 (x2,y2) 在以 (x1,y1) 为中心、半宽 0x50/半高 0x96 的矩形内
#include "CHackLog_InvalidAreaMove/Is_restrictive_commercial_transaction_zone.inc"
}
