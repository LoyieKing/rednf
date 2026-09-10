void Dispatcher_ExchangeSlot::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140966
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x241);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x24);
    guard.finalize(true);
    user->Send(guard);
