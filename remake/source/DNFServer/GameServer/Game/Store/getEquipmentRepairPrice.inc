int getEquipmentRepairPrice(int baseCost, int curEndurance, int maxEndurance,
                            int growthGrade, bool flag, float rate)
{
    if (maxEndurance == 0 || maxEndurance == curEndurance)
        return 0;

    float price = 0;
    price = (float)(((growthGrade + 5) * baseCost) / 10)
            * g_repairRate / (float)maxEndurance * (float)(maxEndurance - curEndurance);
    price *= rate;
    return (int)price;
