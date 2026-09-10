int CBattle_Field::getNextRoundDifficulty(short round)
{
    int diff = (int)getUltimateDifficylty();
    if (round == getBloodMaxRound() - 1 || round == 0)
        return 1;
    return diff;
