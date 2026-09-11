// ============================================================================
// CParty::forced_set_play_result —— ORIG 0x085b26e8：非 state==3 时向全队
// b28d-solo-CParty: AE —— P248档② m_arr50 char[4]→bool[4]（BattleData.h，已 R2 广播）+ P285 空 else{return;} 复现尾 nop 岛；97=97
// 广播 0x22 结算强制包头（byte0/int0/byte0/byte0）。
// ============================================================================
void CParty::forced_set_play_result()
{
    if ((unsigned char)m_state == 3)                    // +0x6a
        return;
    PacketGuard packet;
    packet.put_header(0, 0x22);
    packet.put_byte(0);
    packet.put_int(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.finalize(true);
    send_to_party(packet);
}