// ===== Dispatcher_Compound_Equipment_Upgrade_Card::set_ReturnSendPacket —— ORIG 0x08228a2e =====
// ORIG：put_byte(0) → put_short(param_2=slot) → put_int(param_3=quantity)。
void Dispatcher_Compound_Equipment_Upgrade_Card::set_ReturnSendPacket(
    PacketGuard& pg, unsigned short slot, int quantity) const
{
    pg.put_byte(0);
    pg.put_short((int)slot);
    pg.put_int(quantity);
}