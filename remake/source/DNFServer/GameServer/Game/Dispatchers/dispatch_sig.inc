int Dispatcher_Pause::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8140a1c 形状：bool 短路判定（false=受理）；受理路径 ctor(req) →
    // ctor(res) → read_packet → process_packet（结果存死槽 -0xc） → send_packet → dtor。
    int nProcessRet;
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x52c;
    CMDPacketStruct::_ReqAdvanceAltarPause req;   // ORIG ctor 0x8141ec6
    CMDPacketStruct::_ResAdvanceAltarPause res;   // ORIG ctor 0x8141ef4
    read_packet(packet, req);          // ORIG read_packet
    nProcessRet = process_packet(user, req, res);  // ORIG 结果写 dead slot
    send_packet(user, res);            // ORIG send_packet(0x242)
    (void)nProcessRet;
    return 0;
}