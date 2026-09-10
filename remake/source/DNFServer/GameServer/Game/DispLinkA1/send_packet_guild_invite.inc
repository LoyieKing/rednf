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
