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

    void BreakJar(unsigned int a, unsigned int b, int c);
    void Composition(unsigned int a, unsigned int b, int c);
    void Decomposition(unsigned int a, unsigned int b, int c);
    void UseCoin(unsigned int a, unsigned int b, int c);
    void PvpPlay(unsigned int a, unsigned int b, int c, int d);
    void RankResult(unsigned int a, unsigned int b, int c, int d, int e,
                    int f, int g, GameResultType& result);
    void DungeonClear(unsigned int a, unsigned int b, int c, int d, int e, int f);
    void DungeonGiveUp(unsigned int a, unsigned int b, int c, int d, int e, int f);
    void DungeonFail(int dungeonIdx);
    void UseShusiaService(int value);
    static void PVPPlay2(int a, int b, int c);
};

#endif  // DNF_GAME_STATISTICS_H_
