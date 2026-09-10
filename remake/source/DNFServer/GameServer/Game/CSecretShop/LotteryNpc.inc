int CSecretShop::LotteryNpc(int dungeonIdx, int level, int price)
{
    unsigned long randomSeed = 10000;
    int randVal = m_pRand->randInt(randomSeed);
    int npcIdx = 1000;
    // [b28-L14] P0 清绑：原 `register unsigned char fv asm("eax") … fv ^= 1`
    // 属禁用形，改内联否定直测（P168a① 裸形；char callee 的 sete 物化差记 N）。
    if (!G_CDataManager()->m_secretShop.GetNpcByDungeonIdx(npcIdx, randVal,
                                                           dungeonIdx))
        G_CDataManager()->m_secretShop.GetNpcByDungeonLev(npcIdx, randVal,
                                                          level);
    if (npcIdx != 0 && npcIdx != 1000)
        m_statistic.RecordShow(npcIdx, dungeonIdx, price);
    return npcIdx;
