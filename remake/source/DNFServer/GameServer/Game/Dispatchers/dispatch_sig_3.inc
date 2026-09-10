int Dispatcher_ResetStar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8140f6e 形状同 Dispatcher_Pause::dispatch_sig，另在 read_packet 后
    // 复制 res+0x08 = req+0x10（objdump 0x08140fe4/0x08140fe7）。
    int nProcessRet;
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x5c0;
    CMDPacketStruct::_ReqAdvanceAltarResetStar req;
    CMDPacketStruct::_ResAdvanceAltarResetStar res;
    read_packet(packet, req);                        // ORIG read_packet
    res.m_field08 = req.m_field10;                   // ORIG 0x08140fe4 movz 复制
    nProcessRet = process_packet(user, req, res);    // ORIG 结果写 dead slot
    send_packet(user, res);                          // ORIG send_packet(0x244)
    (void)nProcessRet;
    return 0;
