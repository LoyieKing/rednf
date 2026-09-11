// ============================================================================
// 3. Dispatcher_CallGuildCreateRight::send_packet_guildcreateright
//    ORIG 0x081ffbde：clear + header(0,0xe) + byte(0) + short(1) +
//    MakeItemPacket(INVEN 1, slot 0) + finalize + Send
// ============================================================================
void Dispatcher_CallGuildCreateRight::send_packet_guildcreateright(
    CUser* user, unsigned char errCode)
{
    (void)errCode;  // ORIG 未读取该参数（栈上落 -0x1c 后未使用）
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, packet);
    packet.finalize(true);
    user->Send(packet);
}