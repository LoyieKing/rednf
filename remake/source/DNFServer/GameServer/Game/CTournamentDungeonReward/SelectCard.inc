// ============================================================================
// ORIG 0x08284ee4（报告 SelectCard.md）：cardA/cardB ∈ [0,1]、slotNo ∈ [0,3]
// 校验后，m_rewards[(int)cardA][(int)cardB].m_flag08（movzbl 零扩展读）== 0xff（未选）
// 才写入 slotNo 低字节并返回 1；否则返回 0。
// [b28d-N18a] fresh 复验 66/68：残差 = 二维索引中间寄存器（movsbl 装载序 +
// `mov %eax,%edx` 中转，P30/N18a）。
// [b28d-solo-CTournamentDungeonReward] DIFF 66/68：残 = N18a×2（读/写各一）；
// p167 0=0 CLEAR。
// [SweepCTournament 2026-09-09] 复核 66/68；jcc 逐位核对（js/jle×3 + cmp $0xff
// /jne）与 ORIG 全等 ⇒ N 封顶。
// ============================================================================
bool CTournamentDungeonReward::SelectCard(int slotNo, char cardA, char cardB)
{
    if (cardA < 0 || cardA > 1)
        return false;
    if (cardB < 0 || cardB > 1)
        return false;
    if (slotNo < 0 || slotNo > 3)
        return false;

    // ORIG cmp $0xff; jne fail —— 命中未选槽（==0xff）直落写入体，失败块居尾
    if (static_cast<unsigned char>(
            m_rewards[(int)cardA][(int)cardB].m_flag08) == 0xff)
    {
        m_rewards[(int)cardA][(int)cardB].m_flag08 = static_cast<char>(slotNo);
        return true;
    }
    return false;
}