// ORIG T 0x82e8afc：包 0x86 图标列表 = put_header(0,0x86) + put_short(1) +
// MakeViewAssaultIconOnePlayer + finalize(true)。
void CAssaultPlace::_MakeViewAssaultIconPlayer(PacketGuard* packet, bool flag)
{
    packet->put_header(0, 0x86);
    packet->put_short(1);
    MakeViewAssaultIconOnePlayer(packet, flag);
    packet->finalize(true);
}