void Dispatcher_Skill_Command_Customizing::send(CUser* user, ParamBase& param)
{
    const stSkillCommandParam& res = static_cast<const stSkillCommandParam&>(param);
    if (res.m_4 == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x14c);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x14c);
        packet.put_byte(0);
        packet.put_short((int)res.m_4);
        packet.finalize(true);
        user->Send(packet);
    }
