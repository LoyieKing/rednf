// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHackLog_InvalidAreaMove/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
{
    return IsSeriaRoom(a, b);
}

// ORIG 0x086d272c
//   _ZN24CHackLog_InvalidAreaMove11GetHackTypeEv
//   86d272c: mov $0x1f5,%eax
#include "CHackLog_InvalidAreaMove/GetHackType.inc"
{
    return 0x1f5;
}

// ORIG 0x082861b6
//   _ZN24CHackLog_InvalidAreaMove31IsMovingBetweenWestCoastAndGentEiiii
//   if (IsGentEntranceInWestCoast(a,b) && IsWestCostEntranceInGent(c,d)) return 1;
//   if (IsWestCostEntranceInGent(a,b) && IsGentEntranceInWestCoast(c,d)) return 1;
//   return 0;
#include "CHackLog_InvalidAreaMove/IsMovingBetweenWestCoastAndGent.inc"
{
    // ORIG 直连形：单表达式 return 不物化 0/1（同 IsMovingBetweenSeriaRoomAndGuildAgit）。
    return (IsGentEntranceInWestCoast(a, b) && IsWestCostEntranceInGent(c, d)) ||
           (IsWestCostEntranceInGent(a, b) && IsGentEntranceInWestCoast(c, d));
}

// ORIG 0x082862d0
//   _ZN24CHackLog_InvalidAreaMove25IsGentEntranceInWestCoastEii
//   a == 3 && b == 6
#include "CHackLog_InvalidAreaMove/IsGentEntranceInWestCoast.inc"
{
    return (a == 3 && b == 6) ? 1 : 0;
}

// ORIG 0x082862ee
//   _ZN24CHackLog_InvalidAreaMove24IsWestCostEntranceInGentEii
//   a == 6 && b == 0
#include "CHackLog_InvalidAreaMove/IsWestCostEntranceInGent.inc"
{
    return (a == 6 && b == 0) ? 1 : 0;
}

// ORIG 0x08286222
//   _ZN24CHackLog_InvalidAreaMove21IsMovingAboutPowerWarEii
//   if (IsHendonMyre(a) && IsWarfare(b)) return 1;
//   if (IsWarfare(a) && IsHendonMyre(b)) return 1;
//   return 0;
#include "CHackLog_InvalidAreaMove/IsMovingAboutPowerWar.inc"
{
    // ORIG 直连形：单表达式 return。
    return (IsHendonMyre(a) && IsWarfare(b)) ||
           (IsWarfare(a) && IsHendonMyre(b));
}

// ORIG 0x08286330
//   _ZN24CHackLog_InvalidAreaMove19IsMovingInGuildAgitEii
//   a == 8 && a == b
#include "CHackLog_InvalidAreaMove/IsMovingInGuildAgit.inc"
{
    return (a == 8 && a == b) ? 1 : 0;
}

// ORIG 0x08286350
//   _ZN24CHackLog_InvalidAreaMove15IsExceptionMoveEiiii
//   任一四元组匹配即返回 1：
//   (6,0,6,4) (6,4,6,0) (3,8,3,0) (3,0,3,8) (3,1,2,0)
//   (2,0,3,1) (2,2,2,5) (2,5,2,2) (2,1,2,5) (2,5,2,1)
#include "CHackLog_InvalidAreaMove/IsExceptionMove.inc"
{
    if ((a == 6 && b == 0 && c == 6 && d == 4) ||
        (a == 6 && b == 4 && c == 6 && d == 0) ||
        (a == 3 && b == 8 && c == 3 && d == 0) ||
        (a == 3 && b == 0 && c == 3 && d == 8) ||
        (a == 3 && b == 1 && c == 2 && d == 0) ||
        (a == 2 && b == 0 && c == 3 && d == 1) ||
        (a == 2 && b == 2 && c == 2 && d == 5) ||
        (a == 2 && b == 5 && c == 2 && d == 2) ||
        (a == 2 && b == 1 && c == 2 && d == 5) ||
        (a == 2 && b == 5 && c == 2 && d == 1))
        return 1;
    return 0;
}

// ORIG 0x08286272
//   _ZN24CHackLog_InvalidAreaMove36IsMovingBetweenSeriaRoomAndGuildAgitEiiii
//   if (IsSeriaRoom(a,b) && IsGuildAgit(c)) return 1;
//   if (IsGuildAgit(a) && IsSeriaRoom(c,d)) return 1;
//   return 0;
#include "CHackLog_InvalidAreaMove/IsMovingBetweenSeriaRoomAndGuildAgit.inc"
{
    // ORIG 直连形：A 后 je、B 后 jne return1——单表达式 return 才不物化 0/1。
    return (IsSeriaRoom(a, b) && IsGuildAgit(c)) ||
           (IsGuildAgit(a) && IsSeriaRoom(c, d));
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
{
    // ORIG xor 链形：谓词 bool 声明 + 全链取反 if（call;xor $0x1;test;je 连锁 + 尾物化块）。
    if (!IsMovingBetweenWestCoastAndGent(a, b, c, d) &&
        !IsMovingAboutPowerWar(a, c) &&
        !IsMovingInGuildAgit(a, c) &&
        !IsExceptionMove(a, b, c, d) &&
        !IsMovingBetweenSeriaRoomAndGuildAgit(a, b, c, d))
        return 1;
    return 0;
}

// ORIG 0x08285efe
//   _ZN24CHackLog_InvalidAreaMove14isMovableRouteEiiiiii
//   任一六元组匹配即返回 1，否则 0（局部 char 初值 0，命中置 1）：
//   (3,2,3,0,2,0) (3,1,3,0,2,0) (2,0,3,0,3,1) (2,0,3,0,3,2)
//   (6,0,3,6,3,1) (4,1,4,0,4,3) (4,3,4,0,4,1) (1,3,4,0,4,4)
//   (4,4,4,0,1,3) (2,2,2,8,5,0) (5,0,2,8,2,2) (2,4,2,0,3,0)
//   (4,1,4,0,1,3) (4,0,1,3,1,0) (3,6,6,0,6,1)
#include "CHackLog_InvalidAreaMove/isMovableRoute.inc"
{
    bool result = false;
    if ((a == 3 && b == 2 && c == 3 && d == 0 && e == 2 && f == 0) ||
        (a == 3 && b == 1 && c == 3 && d == 0 && e == 2 && f == 0) ||
        (a == 2 && b == 0 && c == 3 && d == 0 && e == 3 && f == 1) ||
        (a == 2 && b == 0 && c == 3 && d == 0 && e == 3 && f == 2) ||
        (a == 6 && b == 0 && c == 3 && d == 6 && e == 3 && f == 1) ||
        (a == 4 && b == 1 && c == 4 && d == 0 && e == 4 && f == 3) ||
        (a == 4 && b == 3 && c == 4 && d == 0 && e == 4 && f == 1) ||
        (a == 1 && b == 3 && c == 4 && d == 0 && e == 4 && f == 4) ||
        (a == 4 && b == 4 && c == 4 && d == 0 && e == 1 && f == 3) ||
        (a == 2 && b == 2 && c == 2 && d == 8 && e == 5 && f == 0) ||
        (a == 5 && b == 0 && c == 2 && d == 8 && e == 2 && f == 2) ||
        (a == 2 && b == 4 && c == 2 && d == 0 && e == 3 && f == 0) ||
        (a == 4 && b == 1 && c == 4 && d == 0 && e == 1 && f == 3) ||
        (a == 4 && b == 0 && c == 1 && d == 3 && e == 1 && f == 0) ||
        (a == 3 && b == 6 && c == 6 && d == 0 && e == 6 && f == 1))
        result = true;
    return result;
}

// ORIG 0x0828630c
//   _ZN24CHackLog_InvalidAreaMove12IsHendonMyreEi
//   a == 2
#include "CHackLog_InvalidAreaMove/IsHendonMyre.inc"
{
    return a == 2;
}

// ORIG 0x08286318
//   _ZN24CHackLog_InvalidAreaMove9IsWarfareEi
//   a == 7
#include "CHackLog_InvalidAreaMove/IsWarfare.inc"
{
    return a == 7;
}

// ORIG 0x08286324
//   _ZN24CHackLog_InvalidAreaMove11IsGuildAgitEi
//   a == 8
#include "CHackLog_InvalidAreaMove/IsGuildAgit.inc"
{
    return a == 8;
}

// ORIG 0x08286462
//   _ZN24CHackLog_InvalidAreaMove11IsSeriaRoomEii
//   任一 (a,b) 匹配即返回 1：
//   (1,1) (2,5) (3,2) (4,1) (5,1) (6,4) (9,2) (11,3)
#include "CHackLog_InvalidAreaMove/IsSeriaRoom.inc"
{
    // ORIG 0x08286462 岛形：先 (1,1) 独立 return 岛（mov$1;jmp 汇入），其后主链
    // 仍以 (a==1&&b==1) 为第一项；4.4 若只给原链会折掉先导岛，须显式先判一次。
    if (a == 1 && b == 1)
        return 1;
    if ((a == 1 && b == 1) ||
        (a == 2 && b == 5) ||
        (a == 3 && b == 2) ||
        (a == 4 && b == 1) ||
        (a == 5 && b == 1) ||
        (a == 6 && b == 4) ||
        (a == 9 && b == 2) ||
        (a == 11 && b == 3))
        return 1;
    return 0;
}

// ORIG 0x086c4568
//   _Z42Is_restrictive_commercial_transaction_zoneiiii
//   自由函数；点 (x2,y2) 在以 (x1,y1) 为中心、半宽 0x50/半高 0x96 的矩形内
#include "CHackLog_InvalidAreaMove/Is_restrictive_commercial_transaction_zone.inc"
{
    return (y1 - 0x96 < y2) && (y1 + 0x96 > y2) &&
           (x1 - 0x50 < x2) && (x1 + 0x50 > x2);
}
