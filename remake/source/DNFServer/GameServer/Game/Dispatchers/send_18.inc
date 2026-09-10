void Dispatcher_Mercenary_Info::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ddb7c (0xa7 insn): put_header(1,0x1ba) + put_byte(1) +
    // SetMercenaryInfoPacket + finalize(true) + Send
    PacketGuard guard;
    guard.put_header(1, 0x1ba);
    guard.put_byte(1);
    user->SetMercenaryInfoPacket(guard);
    guard.finalize(true);
    user->Send(guard);
