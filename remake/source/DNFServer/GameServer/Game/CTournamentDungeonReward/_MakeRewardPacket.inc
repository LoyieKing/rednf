// ORIG 0x82852e4：单卡型奖励包体（byte2 + 每卡槽 int/int/short0）
// [b28d-N18a] fresh 复验 65/66：残 1 行 = 二维索引中间寄存器 + 块尾 nop 差。
// [b28d-solo-CTournamentDungeonReward] DIFF 65/66：残 = N18a×2 + ORIG 空 then 占位
// nop 岛（§6.1 剥离）；p167 4=4 CLEAR。
void CTournamentDungeonReward::_MakeRewardPacket(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, PacketGuard* packet)
{
    // [SweepCTournament 2026-09-09] P223：ORIG 0x082853ae 尾 `jmp +0xcd` +
    // 0x082853b0 `nop`（jg 的落点）＝4.1 空 then 占位岛；`if((int)type<2){...}`
    // 被 4.4 jump-thread（少 jmp+nop，diff 实证 -2）。复现形＝否定 goto + 尾空标签
    // （探针 /tmp/probe_isl.cpp F/G：F 无岛、G 产 `jne; body; jmp; nop` 逐条同形）。
    if ((int)type >= 2)
        goto L_done;
    {
        packet->put_byte(2);
        for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
        {
            packet->put_int(m_rewards[(int)type][slotIdx].m_id);
            packet->put_int(m_rewards[(int)type][slotIdx].m_field04);
            packet->put_short(0);
        }
    }
L_done:;
}