void Dispatcher_SeriaRoom_Deco_Event::send(CUser* user, ParamBase& param)
{
    stSeriaRoomDecoEvent& out = static_cast<stSeriaRoomDecoEvent&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1ef);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_short((unsigned int)(unsigned short)out.m_8);
        packet.put_short((int)out.m_a);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
