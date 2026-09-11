// CBattle_Field::getNextRoundRandomDifficulty —— ORIG 0x08306156（0x5f 字节）
//   报告：docs/class_func_reports/CBattle_Field/getNextRoundRandomDifficulty.md
//   大轮序==最大轮-1 → 1；否则掷 100，<=49 → 2，否则 1。
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
}