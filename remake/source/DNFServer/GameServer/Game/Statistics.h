#ifndef DNF_GAME_STATISTICS_H_
#define DNF_GAME_STATISTICS_H_

// ============================================================================
// df_game_r Game/ Statistics（统计上报聚合类，独立 TU 实现于 Statistics.cpp）。
// 唯一声明点（原 Statistics.cpp 本地类迁出，PvP_Room.cpp 等 TU include 使用）。
// ORIG 符号见 docs/class_func_reports/Statistics/（PVPPlay2 0x0860b8d8 等）。
// 注意：PVPPlay2 在 ORIG 调用点（start_pvp 085d8fde）为无 this 压栈的自由调用
// 形态，故声明为 static（mangled 名 _ZN10Statistics8PVPPlay2Eiii 不变）。
// ============================================================================

class GameResultType;

class Statistics
{
public:
    Statistics();
    ~Statistics();

    static void SendSignal(int signal, void* data, int len);

    static void BreakJar(unsigned int a, unsigned int b, int c);      // ORIG 参数槽 a@0x8 起、无 this（static mangled 同形）
    static void Composition(unsigned int a, unsigned int b, int c);
    static void Decomposition(unsigned int a, unsigned int b, int c);
    static void UseCoin(unsigned int a, unsigned int b, int c);
    static void PvpPlay(unsigned int a, unsigned int b, int c, int d);
    static void RankResult(unsigned int a, unsigned int b, int c, int d, int e,
                    int f, int g, GameResultType& result);
    static void DungeonClear(unsigned int a, unsigned int b, int c, int d, int e, int f);
    static void DungeonGiveUp(unsigned int a, unsigned int b, int c, int d, int e, int f);
    static void DungeonFail(int dungeonIdx);   // ORIG 调用点无 this（CParty::giveup_game 0x85b2d1c 仅压 1 参）→ 静态成员
    static void UseShusiaService(int value);   // ORIG 调用点无 this（serviceOfShusia@0x8657ac8 仅压 1 参）→ 静态成员
    static void PVPPlay2(int a, int b, int c);
};

#endif  // DNF_GAME_STATISTICS_H_
