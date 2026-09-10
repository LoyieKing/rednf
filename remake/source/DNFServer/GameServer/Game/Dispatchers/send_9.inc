void Dispatcher_CollectItems::send(CUser* user, ParamBase& param)
{
    stCollectItems& out = static_cast<stCollectItems&>(param);
    if (out.m_4 == (int)0x7fffffff)
    {
        return;
    }
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e3,
                                 (unsigned char)(out.m_4 & 0xff));
        return;
    }
    PacketGuard packet;
    packet.put_header(1, 0x1e3);
    packet.put_byte(1);
    packet.put_short((int)out.m_8);
    packet.put_short((unsigned short)out.m_a);
    packet.finalize(true);
    user->Send(packet);
