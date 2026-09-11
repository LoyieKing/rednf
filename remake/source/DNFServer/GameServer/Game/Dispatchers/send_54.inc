void Dispatcher_ChangeAnotherSkillTree::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d2134：p4==0 → header(1,0x108)+byte 1+byte treeIdx；否则 SendCmdErrorPacket(0x108,p4&0xff)
    stChangeAnotherSkillTree& p = static_cast<stChangeAnotherSkillTree&>(param);
    if (p.m_result == 0)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0x108);
        pkt.put_byte(1);
        pkt.put_byte((int)(char)p.m_skillTreeIndex);
        pkt.finalize(true);
        user->Send(pkt);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x108,
                                 (unsigned char)(p.m_result & 0xff));
    }
}