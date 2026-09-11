// ---------------------------------------------------------------------------
// send_to_party_cmd_error —— ORIG T 0x0859d1aa
// 报告：docs/class_func_reports/CParty/send_to_party_cmd_error.md
// put_header(1, cmd) + byte 0 + byte error + finalize(true) 后全队发送。
// ---------------------------------------------------------------------------
void CParty::send_to_party_cmd_error(ENUM_CMDPACKET cmd, unsigned char error)
{
    PacketGuard packet;
    packet.put_header(1, cmd);
    packet.put_byte(0);
    packet.put_byte(error);
    packet.finalize(true);
    send_to_party(packet);
}