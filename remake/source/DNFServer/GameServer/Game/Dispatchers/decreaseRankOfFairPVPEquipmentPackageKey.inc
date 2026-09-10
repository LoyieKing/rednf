int decreaseRankOfFairPVPEquipmentPackageKey(int key)
{
    // ORIG：整槽 -0x4 = key，槽高 16 位 -0x2 减 1 写回后返回整槽（低 16 位保留）。
    union
    {
        int k;
        unsigned short w[2];
    } u;
    u.k = key;
    u.w[1] = static_cast<unsigned short>(u.w[1] - 1);   // ORIG 无下溢保护（wrap）
    return u.k;
