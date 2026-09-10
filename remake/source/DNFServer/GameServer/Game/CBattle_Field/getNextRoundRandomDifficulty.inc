int CBattle_Field::getNextRoundRandomDifficulty()
{
    if ((short)m_bloodRound.getBloodRoundOrder() == getBloodMaxRound() - 1)
    {
        return 1;
    }
    else
    {
        int r = get_rand_int(100);
        if (r > 49)
            return 1;
        return 2;
    }
