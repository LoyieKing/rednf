// ---- GetQuestItemDropRate（报告，ORIG T 0x085d12ee sz 0x71）----
int CAlchemist::GetQuestItemDropRate(CUser* user, int rate)
{
    // ORIG 0x085d12ee：A || B 短路合并（jne→mov$1 / je→mov$0 + test，boosted 不落槽）
    if (IsExistContinuousEffectItem(user, 0x27ac52) ||
        IsExistContinuousEffectItem(user, 0x1f8e))
    {
        return (g_alchemistInfo[1] + 10000) * rate / 10000;
    }
    return rate;
}