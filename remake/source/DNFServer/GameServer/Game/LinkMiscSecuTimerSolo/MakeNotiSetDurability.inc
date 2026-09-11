// ===========================================================================
// MakeNotiSetDurability  ORIG T 0x082f84f8：header(0,0xb3) + put_int + finalize(1)。
// ===========================================================================
void MakeNotiSetDurability(PacketGuard& packet, int value)
{
    packet.put_header(0, 0xb3);
    packet.put_int(value);
    packet.finalize(true);
}