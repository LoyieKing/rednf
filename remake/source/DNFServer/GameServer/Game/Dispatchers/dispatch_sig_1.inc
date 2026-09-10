int Dispatcher_BuyItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x813fad0 形状同 Dispatcher_Pause::dispatch_sig（见其注释）。
    int nProcessRet;
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x380;
    CMDPacketStruct::_ReqAdvanceAltarBuyItem req;
    CMDPacketStruct::_ResAdvanceAltarBuyItem res;
    read_packet(packet, req);              // ORIG read_packet
    nProcessRet = process_packet(user, req, res);  // ORIG 结果写 dead slot
    send_packet(user, res);                // ORIG send_packet(0x23d)
    (void)nProcessRet;
    return 0;
