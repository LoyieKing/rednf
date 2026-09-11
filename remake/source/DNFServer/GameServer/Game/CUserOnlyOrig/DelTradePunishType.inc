// ---------------------------------------------------------------------------
// 20. CUser::DelTradePunishType(int) T 0x086898c2
void CUser::DelTradePunishType(int mask)
{
    printf("DelTradePunishType : (%d) // Prev TradePunishType:  %d\n", mask, m_postChecksumData.m_tradePunishType);
    // ORIG 0x086898e8-0x086898fb：member→%edx、%eax=~mask、and %eax,%edx。
    // probe8 d2 实证仅 unsigned 成员复现此寄存器分配（int 成员 dest=~mask 形）。
    m_postChecksumData.m_tradePunishType &= ~mask;
    printf("After TradePunishType:  %d\n", m_postChecksumData.m_tradePunishType);
}