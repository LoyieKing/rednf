void CAssaultPlace::_MakeViewAssaultIconPlayer(PacketGuard* packet, bool flag)
{
    packet->put_header(0, 0x86);
    packet->put_short(1);
    MakeViewAssaultIconOnePlayer(packet, flag);
    packet->finalize(true);
