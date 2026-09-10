void Dispatcher_SetSlot::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140148
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x23e);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x1c);
    guard.finalize(true);
    user->Send(guard);
