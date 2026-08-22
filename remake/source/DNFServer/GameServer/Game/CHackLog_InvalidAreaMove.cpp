#include "CHackLog_InvalidAreaMove.h"

// 全部方法按 ORIG df_game_r（elf32-i386）逐指令反汇编转写。
// 反汇编命令：objdump -d /home/wangyilei/下载/df_game_r
// 地址与符号见各方法注释。

// ORIG 0x086d272c
//   _ZN24CHackLog_InvalidAreaMove11GetHackTypeEv
//   86d272c: mov $0x1f5,%eax
int CHackLog_InvalidAreaMove::GetHackType()
{
    return 0x1f5;
}

// ORIG 0x082862d0
//   _ZN24CHackLog_InvalidAreaMove25IsGentEntranceInWestCoastEii
//   a == 3 && b == 6
char CHackLog_InvalidAreaMove::IsGentEntranceInWestCoast(int a, int b)
{
    return (a == 3 && b == 6) ? 1 : 0;
}

// ORIG 0x082862ee
//   _ZN24CHackLog_InvalidAreaMove24IsWestCostEntranceInGentEii
//   a == 6 && b == 0
char CHackLog_InvalidAreaMove::IsWestCostEntranceInGent(int a, int b)
{
    return (a == 6 && b == 0) ? 1 : 0;
}

// ORIG 0x082861b6
//   _ZN24CHackLog_InvalidAreaMove31IsMovingBetweenWestCoastAndGentEiiii
//   if (IsGentEntranceInWestCoast(a,b) && IsWestCostEntranceInGent(c,d)) return 1;
//   if (IsWestCostEntranceInGent(a,b) && IsGentEntranceInWestCoast(c,d)) return 1;
//   return 0;
char CHackLog_InvalidAreaMove::IsMovingBetweenWestCoastAndGent(int a, int b, int c, int d)
{
    if (IsGentEntranceInWestCoast(a, b) && IsWestCostEntranceInGent(c, d))
        return 1;
    if (IsWestCostEntranceInGent(a, b) && IsGentEntranceInWestCoast(c, d))
        return 1;
    return 0;
}

// ORIG 0x08286222
//   _ZN24CHackLog_InvalidAreaMove21IsMovingAboutPowerWarEii
//   if (IsHendonMyre(a) && IsWarfare(b)) return 1;
//   if (IsWarfare(a) && IsHendonMyre(b)) return 1;
//   return 0;
char CHackLog_InvalidAreaMove::IsMovingAboutPowerWar(int a, int b)
{
    if (IsHendonMyre(a) && IsWarfare(b))
        return 1;
    if (IsWarfare(a) && IsHendonMyre(b))
        return 1;
    return 0;
}

// ORIG 0x08286330
//   _ZN24CHackLog_InvalidAreaMove19IsMovingInGuildAgitEii
//   a == 8 && a == b
char CHackLog_InvalidAreaMove::IsMovingInGuildAgit(int a, int b)
{
    return (a == 8 && a == b) ? 1 : 0;
}

// ORIG 0x08286350
//   _ZN24CHackLog_InvalidAreaMove15IsExceptionMoveEiiii
//   任一四元组匹配即返回 1：
//   (6,0,6,4) (6,4,6,0) (3,8,3,0) (3,0,3,8) (3,1,2,0)
//   (2,0,3,1) (2,2,2,5) (2,5,2,2) (2,1,2,5) (2,5,2,1)
char CHackLog_InvalidAreaMove::IsExceptionMove(int a, int b, int c, int d)
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
char CHackLog_InvalidAreaMove::IsMovingBetweenSeriaRoomAndGuildAgit(int a, int b, int c, int d)
{
    if (IsSeriaRoom(a, b) && IsGuildAgit(c))
        return 1;
    if (IsGuildAgit(a) && IsSeriaRoom(c, d))
        return 1;
    return 0;
}

// ORIG 0x08285e14
//   _ZN24CHackLog_InvalidAreaMove14checkSeriaRoomEii
//   直接返回 IsSeriaRoom(a, b) 的结果（eax 保留）。
char CHackLog_InvalidAreaMove::checkSeriaRoom(int a, int b)
{
    return IsSeriaRoom(a, b);
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
char CHackLog_InvalidAreaMove::IsInvalidMovableArea(int a, int b, int c, int d)
{
    if (IsMovingBetweenWestCoastAndGent(a, b, c, d))
        return 0;
    if (IsMovingAboutPowerWar(a, c))
        return 0;
    if (IsMovingInGuildAgit(a, c))
        return 0;
    if (IsExceptionMove(a, b, c, d))
        return 0;
    if (IsMovingBetweenSeriaRoomAndGuildAgit(a, b, c, d))
        return 0;
    return 1;
}

// ORIG 0x08285efe
//   _ZN24CHackLog_InvalidAreaMove14isMovableRouteEiiiiii
//   任一六元组匹配即返回 1，否则 0（局部 char 初值 0，命中置 1）：
//   (3,2,3,0,2,0) (3,1,3,0,2,0) (2,0,3,0,3,1) (2,0,3,0,3,2)
//   (6,0,3,6,3,1) (4,1,4,0,4,3) (4,3,4,0,4,1) (1,3,4,0,4,4)
//   (4,4,4,0,1,3) (2,2,2,8,5,0) (5,0,2,8,2,2) (2,4,2,0,3,0)
//   (4,1,4,0,1,3) (4,0,1,3,1,0) (3,6,6,0,6,1)
char CHackLog_InvalidAreaMove::isMovableRoute(int a, int b, int c, int d, int e, int f)
{
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
        return 1;
    return 0;
}

// ORIG 0x0828630c
//   _ZN24CHackLog_InvalidAreaMove12IsHendonMyreEi
//   a == 2
char CHackLog_InvalidAreaMove::IsHendonMyre(int a)
{
    return (a == 2) ? 1 : 0;
}

// ORIG 0x08286318
//   _ZN24CHackLog_InvalidAreaMove9IsWarfareEi
//   a == 7
char CHackLog_InvalidAreaMove::IsWarfare(int a)
{
    return (a == 7) ? 1 : 0;
}

// ORIG 0x08286324
//   _ZN24CHackLog_InvalidAreaMove11IsGuildAgitEi
//   a == 8
char CHackLog_InvalidAreaMove::IsGuildAgit(int a)
{
    return (a == 8) ? 1 : 0;
}

// ORIG 0x08286462
//   _ZN24CHackLog_InvalidAreaMove11IsSeriaRoomEii
//   任一 (a,b) 匹配即返回 1：
//   (1,1) (2,5) (3,2) (4,1) (5,1) (6,4) (9,2) (11,3)
char CHackLog_InvalidAreaMove::IsSeriaRoom(int a, int b)
{
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
bool Is_restrictive_commercial_transaction_zone(int x1, int y1, int x2, int y2)
{
    return (y1 - 0x96 < y2) && (y1 + 0x96 > y2) &&
           (x1 - 0x50 < x2) && (x1 + 0x50 > x2);
}
