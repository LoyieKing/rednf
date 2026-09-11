// ---- GetCommissionPercent（ORIG T 0x85c9b8c，报告 GetCommissionPercent.md 全文）----
// 频道类型 3 → 3%，否则 4%。
int CPrivateStore::GetCommissionPercent()
{
    if (G_GameWorld()->GetChannelType() == 3)
        return 3;
    return 4;
}