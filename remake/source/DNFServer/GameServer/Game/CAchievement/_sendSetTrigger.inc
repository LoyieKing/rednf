void CAchievement::_sendSetTrigger(unsigned int id, unsigned char flag)
{
    stAchievement trig = _getTrigger(id);

    PacketGuard packet;
    packet.put_header(1, 0x1a1);
    // ORIG 0x828c2a2：cmpb flag; je 跳到 flag==0 分支，flag!=0 分支为 fall-through
    if (flag != 0)
    {
        packet.put_byte(0);
        packet.put_byte((int)flag);
        packet.put_int((int)trig.m_id);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int((int)trig.m_id);
        packet.put_short((int)trig.m_trigger0);
        packet.put_short((int)trig.m_trigger1);
        packet.put_short((int)trig.m_trigger2);
    }
    packet.finalize(true);
    m_user->Send(packet);
