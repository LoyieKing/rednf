void Dispatcher_BingoReward::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825e87a：param+4==0 → SendCmdOkPacket(0x202)；否则 header(1,0x202)+byte 0+byte err+int data
    stBingoReward& p = static_cast<stBingoReward&>(param);
    if (p.m_result == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x202);
        return;
    }
    PacketGuard pkt;
    pkt.put_header(1, 0x202);
    pkt.put_byte(0);
    pkt.put_byte(p.m_result);
    pkt.put_int(p.m_bingoData);
    pkt.finalize(true);
    user->Send(pkt);
}