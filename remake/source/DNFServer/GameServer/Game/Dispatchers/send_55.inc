void Dispatcher_CharacSlotExtendEffect::send(CUser* user, ParamBase& param)
{
    stCharacSlotExtendEffect& out = static_cast<stCharacSlotExtendEffect&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x19f);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte((unsigned int)(unsigned char)out.m_9);
        packet.put_byte((unsigned int)(unsigned char)out.m_8);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
