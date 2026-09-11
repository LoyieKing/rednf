// ---------------------------------------------------------------------------
// CBattle_Field::getNextRoundDifficulty(short) —— ORIG T 0x083061b6：
//   下一轮难度 = getUltimateDifficylty()；最后一轮或第 0 轮=1。
// [b24F] DONE：if 内 diff=1 改 return 1（ORIG mov $0x1,%eax 直返）。
// ---------------------------------------------------------------------------
int CBattle_Field::getNextRoundDifficulty(short round)
{
    int diff = (int)getUltimateDifficylty();
    if (round == getBloodMaxRound() - 1 || round == 0)
        return 1;
    return diff;
}