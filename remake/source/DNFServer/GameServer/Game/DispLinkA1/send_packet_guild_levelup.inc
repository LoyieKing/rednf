// ============================================================================
// 5. Dispatcher_CallGuildLevelUp::send_packet_guild_levelup  ORIG 0x08200126
//    同 4 型：header(1,0x4c)
// [b28d-solo-DispLinkA1] verdict=IDENTICAL（58=58，纯重定位标签差）。
// ============================================================================
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
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
}