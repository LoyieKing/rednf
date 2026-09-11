// ============================================================================
// ORIG 0x08285284（报告 CTournamentDungeonReward/isMemberSelectCard.md）：
// 遍历该卡型两槽，m_flag08（char 符号扩展）== cardNo 即 1。
// [b28c-16 定论 N18a] 残差仅二维寻址中间结果寄存器：ORIG `lea 0x0(,%eax,4),%ecx`
// 直落 ecx（type 在 edx/slot 在 ecx），4.4 恒 slot*12→eax 后 `mov %eax,%edx`
// 多 1 条并改寄存器名（type→ecx/slot→eax）。探针 /tmp/l16p/p8.cpp 两形不可择。
// [b28d-N18a] fresh 复验 34/35：维持 N18a。
// [b28d-solo-CTournamentDungeonReward] DIFF 34/35：残 = N18a×1；p167 0=0 CLEAR。
// [SweepCTournament 2026-09-09] 复核 34/35；jcc（jne/setle）与 ORIG 全等 ⇒ N。
// ============================================================================
bool CTournamentDungeonReward::isMemberSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, int cardNo)
{
    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<char>(m_rewards[(int)type][slot].m_flag08) == cardNo)
            return true;
    }
    return false;
}