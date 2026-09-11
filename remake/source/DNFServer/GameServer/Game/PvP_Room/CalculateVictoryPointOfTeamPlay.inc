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