void Dispatcher_BuyItem::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x813fe82
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x23d);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x1c);
    guard.finalize(true);
    user->Send(guard);
}