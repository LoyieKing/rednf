// [b28d-7 N97] ORIG 0x085da086：有符号 /2^k 为 mov/sar/shr/lea 四段，4.4 用 mov/test/cmovs 合并形（N97 源形不可造）⇒ 封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] N97: ORIG 0x085da086 有符号 /2^k 为 mov/sar/shr/lea 四段，4.4 用 cmovs 合并形
int PvP_Room::CalculateVictoryPointOfSinglePlay(int rank, bool win, int score)
{
    int local_10;
    if (win)
    {
        return score + 1;                 // ORIG win 分支直返（不写 local_10）
    }
    else if (rank == 1)
    {
        local_10 = (score + 1) / 2;
    }
    else if (rank == 2)
    {
        local_10 = (score + 1) / 4;
    }
    else if (rank == 3)
    {
        local_10 = (score + 1) / 8;
    }
    else if (rank == 4)
    {
        local_10 = (score + 1) / 16;
    }
    else if (rank == 5)
    {
        local_10 = (score + 1) / 32;
    }
    else if (rank == 6)
    {
        local_10 = (score + 1) / 64;
    }
    else if (rank == 7)
    {
        local_10 = (score + 1) / 128;
    }
    else if (rank == 0)
    {
        local_10 = 1;
    }
    else
    {
        cMyTrace local_20(__PRETTY_FUNCTION__,
                          0xb50, 5);
        local_20("invalid rank %d", rank);
    }
    return local_10;
}