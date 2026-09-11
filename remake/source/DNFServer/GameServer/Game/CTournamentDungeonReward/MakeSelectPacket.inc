// ============================================================================
// ORIG 0x08284fa4（报告 MakeSelectPacket.md）：两轮 put_byte(2)；内轮对两槽
// put_byte((char)m_flag08 符号扩展)。
// [b28c-16 定论 N18a] 同上（二维寻址中间寄存器单段直落不可造）。
// [b28d-N18a] fresh 复验 44/45：维持 N18a。
// [b28d-solo-CTournamentDungeonReward] DIFF 44/45：残 = N18a×1；p167 2=2 CLEAR。
// [SweepCTournament 2026-09-09] 复核 44/45；put_byte 调用点 2=2 全等 ⇒ N。
// ============================================================================
void CTournamentDungeonReward::MakeSelectPacket(PacketGuard* packet)
{
    for (int type = 0; type < 2; ++type)
    {
        packet->put_byte(2);
        for (int slot = 0; slot < 2; ++slot)
        {
            packet->put_byte(static_cast<char>(m_rewards[type][slot].m_flag08));
        }
    }
}