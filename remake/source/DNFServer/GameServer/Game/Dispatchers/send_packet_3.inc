void Dispatcher_ResetStar::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81411fc
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x244);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0xc);
    guard.finalize(true);
    user->Send(guard);
