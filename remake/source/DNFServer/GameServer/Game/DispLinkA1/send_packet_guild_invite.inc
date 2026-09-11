// ============================================================================
// 4. Dispatcher_CallGuildInvite::send_packet_guild_invite  ORIG 0x08207852
//    header(1,0x9a)；inviteType==0 → byte(1)，否则 byte(0)+byte(inviteType)
// ============================================================================
void Dispatcher_CallGuildInvite::send_packet_guild_invite(
    CUser* user, unsigned char inviteType)
{
    PacketGuard packet;
    packet.put_header(1, 0x9a);
    if (inviteType == 0) {
        packet.put_byte(1);
    }
    else {
        packet.put_byte(0);
        packet.put_byte((int)inviteType);
    }
    packet.finalize(true);
    user->Send(packet);
}