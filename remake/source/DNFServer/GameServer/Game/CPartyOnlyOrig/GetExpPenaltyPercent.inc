// ORIG T 0x085a2320（GetExpPenaltyPercent.md）：等级差惩罚百分比表。
int CParty::GetExpPenaltyPercent(int a, int b) const
{
    if (a < 20)
    {
        unsigned int diff = a - b > 0 ? a - b : b - a;
        if (5 < diff && diff < 10)
        {
            return (10 - diff) * 20;
        }
        if (diff > 9)
        {
            return 5;
        }
    }
    else
    {
        if (a < b)
        {
            return (a * 100) / b;
        }
        unsigned int diff = a - b;
        if (5 < diff && diff < 10)
        {
            return (10 - diff) * 20;
        }
        if (9 < (a - b))
        {
            return 5;
        }
    }
    return 100;
}