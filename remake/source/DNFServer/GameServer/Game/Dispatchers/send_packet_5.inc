void Dispatcher_UpgradeGage::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81403e2
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x23f);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x14);
    guard.finalize(true);
    user->Send(guard);
