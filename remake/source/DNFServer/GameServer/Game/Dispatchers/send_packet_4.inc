void Dispatcher_SummonUnit::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81406b8
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x240);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x14);
    guard.finalize(true);

    user->Send(guard);
}