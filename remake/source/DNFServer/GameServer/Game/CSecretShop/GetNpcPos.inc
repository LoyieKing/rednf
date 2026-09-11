int CSecretShopStatistic::GetNpcPos(int npcIdx)
{
    if (npcIdx == 0x3ea)
        return 0;
    if (npcIdx == 0x3eb)
        return 1;
    if (npcIdx == 0x3ec)
        return 2;
    return 0;
}