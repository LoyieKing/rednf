int Dispatcher_UpgradeGage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x814020e 形状同 Dispatcher_Pause::dispatch_sig（见其注释）。
    int nProcessRet;
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x446;
    CMDPacketStruct::_ReqAdvanceAltarUpgradeGage req;
    CMDPacketStruct::_ResAdvanceAltarUpgradeGage res;
    read_packet(packet, req);              // ORIG read_packet
    nProcessRet = process_packet(user, req, res);  // ORIG 结果写 dead slot
    send_packet(user, res);                // ORIG send_packet(0x23f)
    (void)nProcessRet;
    return 0;
