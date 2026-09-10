// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::CalculateVictoryPointOfTeamPlay（原 TU 第 364 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::CalculateVictoryPointOfTeamPlay(int rank, bool win, int score)
{
    // ORIG 085da1f0/085da203：两条路径均不改写 score 形参槽（0x14(%ebp) 只读），
    // 用 return 表达式而非赋值后统一返回。
    if ((win) && (rank == 0))
    {
        return score + 2;
    }
    else if (!win)
    {
        return 1;
    }
    return score;
}
