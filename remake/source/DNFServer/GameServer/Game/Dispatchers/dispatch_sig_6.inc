int Dispatcher_ExchangeSlot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x814073e 形状同 Dispatcher_Pause::dispatch_sig（见其注释）。
    int nProcessRet;
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x4e8;
    CMDPacketStruct::_ReqAdvanceAltarExchangeSlot req;
    CMDPacketStruct::_ResAdvanceAltarExchangeSlot res;
    read_packet(packet, req);              // ORIG read_packet
    nProcessRet = process_packet(user, req, res);  // ORIG 结果写 dead slot
    send_packet(user, res);                // ORIG send_packet(0x241)
    (void)nProcessRet;
    return 0;
