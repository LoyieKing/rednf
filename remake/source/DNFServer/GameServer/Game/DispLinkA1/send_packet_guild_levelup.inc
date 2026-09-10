void Dispatcher_CallGuildLevelUp::send_packet_guild_levelup(
    CUser* user, unsigned char errCode)
{
    PacketGuard packet;
    packet.put_header(1, 0x4c);
    if (errCode == 0) {
        packet.put_byte(1);
    }
    else {
        packet.put_byte(0);
        packet.put_byte((int)errCode);
    }
    packet.finalize(true);
    user->Send(packet);
