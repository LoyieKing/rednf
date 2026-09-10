#ifndef CHACKLOG_INVALIDAREAMOVE_H
#define CHACKLOG_INVALIDAREAMOVE_H

// CHackLog_InvalidAreaMove
// 从 ORIG 符号 _ZN24CHackLog_InvalidAreaMove* 反汇编转写（类名长度前缀为 24）。
// 全部方法为 static 成员；类无数据成员。
class CHackLog_InvalidAreaMove
{
public:
    // 供 GameWorld.cpp IsInvalidMovableArea 流程引用的外部谓词
    static int  GetHackType();
    static bool IsGentEntranceInWestCoast(int a, int b);
    static bool IsWestCostEntranceInGent(int a, int b);
    static bool IsMovingBetweenWestCoastAndGent(int a, int b, int c, int d);
    static bool IsMovingAboutPowerWar(int a, int b);
    static bool IsMovingInGuildAgit(int a, int b);
    static bool IsExceptionMove(int a, int b, int c, int d);
    static bool IsMovingBetweenSeriaRoomAndGuildAgit(int a, int b, int c, int d);
    static bool checkSeriaRoom(int a, int b);   // ORIG 0x08284250 调用处 test %al;je 无 setne ⇒ bool（char/int 必物化 setne）
    static bool IsInvalidMovableArea(int a, int b, int c, int d);
    // [L11·P248 档①/P292] ORIG 调用点（GameWorld::move_area 0x086c629x）
    // `call; xor $0x1,%eax; test %al,%al; je` = bool 直推导数（无 setne/cmp）
    // ⇒ 原码返回 bool；char 令 4.4 产 `test %al,%al; jne`（缺 xor）。mangled 不含
    // 返回类型 ⇒ 符号/布局零变化。全仓调用点仅 GameWorld.cpp:2636（已 grep）。
    static bool isMovableRoute(int a, int b, int c, int d, int e, int f);

private:
    // ORIG 内部辅助（弱符号，仅供上述方法内部调用）
    static char IsHendonMyre(int a);
    static char IsWarfare(int a);
    static char IsGuildAgit(int a);
    static bool IsSeriaRoom(int a, int b);   // ORIG 0x08285e14 %eax 透传 bool→bool、体内 mov $0x1/$0x0,%eax ⇒ bool
};

// ORIG 0x086c4568：商业交易限制区域判定（自由函数；定义见 CHackLog_InvalidAreaMove.cpp）
bool Is_restrictive_commercial_transaction_zone(int x1, int y1, int x2, int y2);

#endif // CHACKLOG_INVALIDAREAMOVE_H
