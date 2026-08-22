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
    static char IsGentEntranceInWestCoast(int a, int b);
    static char IsWestCostEntranceInGent(int a, int b);
    static char IsMovingBetweenWestCoastAndGent(int a, int b, int c, int d);
    static char IsMovingAboutPowerWar(int a, int b);
    static char IsMovingInGuildAgit(int a, int b);
    static char IsExceptionMove(int a, int b, int c, int d);
    static char IsMovingBetweenSeriaRoomAndGuildAgit(int a, int b, int c, int d);
    static char checkSeriaRoom(int a, int b);
    static char IsInvalidMovableArea(int a, int b, int c, int d);
    static char isMovableRoute(int a, int b, int c, int d, int e, int f);

private:
    // ORIG 内部辅助（弱符号，仅供上述方法内部调用）
    static char IsHendonMyre(int a);
    static char IsWarfare(int a);
    static char IsGuildAgit(int a);
    static char IsSeriaRoom(int a, int b);
};

// ORIG 0x086c4568：商业交易限制区域判定（自由函数；定义见 CHackLog_InvalidAreaMove.cpp）
bool Is_restrictive_commercial_transaction_zone(int x1, int y1, int x2, int y2);

#endif // CHACKLOG_INVALIDAREAMOVE_H
