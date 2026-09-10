int Dispatcher_GetAchievementReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8140c64 形状同 Dispatcher_Pause::dispatch_sig，另在 read_packet 后
    // 复制 res+0x08 = req+0x10（objdump 0x08140cda/0x08140cdd；res+0x08 即
    // m_nAchievementIdx，send_packet 消费）。
    int nProcessRet;
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x571;
    CMDPacketStruct::_ReqAdvanceAltarGetAchievementReward req;
    CMDPacketStruct::_ResAdvanceAltarGetAchievementReward res;
    read_packet(packet, req);                        // ORIG read_packet
    res.m_nAchievementIdx = req.m_field10;           // ORIG 0x08140cda 复制
    nProcessRet = process_packet(user, req, res);    // ORIG process_packet（空体）
    send_packet(user, res);                          // ORIG send_packet(0x243)
    (void)nProcessRet;
    return 0;
