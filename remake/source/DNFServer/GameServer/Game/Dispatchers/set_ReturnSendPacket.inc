void Dispatcher_Compound_Equipment_Upgrade_Card::set_ReturnSendPacket(
    PacketGuard& pg, unsigned short slot, int quantity) const
{
    pg.put_byte(0);
    pg.put_short((int)slot);
    pg.put_int(quantity);
