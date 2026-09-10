void Dispatcher_PcRoomPlayTimeReward::send(CUser* user, ParamBase& param)
{
    PcRoomPlayTimeRewardParam& out = static_cast<PcRoomPlayTimeRewardParam&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1ed);
    if (out.m_result == 0)
    {
        packet.put_byte(1);
        packet.put_byte(user->getPcRoomPlayTimeRewardCnt());
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_result);
        if (out.m_result == 0x13)
        {
            packet.put_int(user->getTotalPcRoomPlayTime());
        }
    }
    packet.finalize(true);
    user->Send(packet);
