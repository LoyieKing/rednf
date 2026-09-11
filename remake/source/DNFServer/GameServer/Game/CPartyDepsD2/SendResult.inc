// ---------------------------------------------------------------------------
// DisPatcher_SetPlayResult::SendResult —— ORIG T 0x081f5db6：
//   header 0x22 结算广播（排名/时间/奖励分/最佳时间+新记录）。
// ---------------------------------------------------------------------------
int DisPatcher_SetPlayResult::SendResult(CParty* party, GameResultSet& result,
                                         int step)
{
    (void)step;
    PacketGuard packet;
    packet.put_header(0, 0x22);
    packet.put_byte((int)(unsigned short)result.m_field48);
    packet.put_int(result.m_fieldc);
    packet.put_byte((int)(char)result.m_field10);
    packet.put_byte((int)(char)result.m_field9);
    party->makeBestClearTimePacket(packet, result.m_fieldc);
    packet.finalize(true);
    party->send_to_party(packet);
    return 1;
}