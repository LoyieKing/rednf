void Dispatcher_Pause::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140bae
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x242);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0xc);
    guard.finalize(true);
    user->Send(guard);
