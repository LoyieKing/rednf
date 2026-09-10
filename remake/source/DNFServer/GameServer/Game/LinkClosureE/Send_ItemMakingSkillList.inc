int CItemMakingSkill::Send_ItemMakingSkillList(CUser* user) const
{
    PacketGuard packet;
    packet.put_header(0, 0x15e);
    packet.put_byte(0xb);
    for (int i = 0; i <= 0xa; ++i)
    {
        packet.put_byte(i);
        packet.put_short((short)m_skillLevels[i]);
    }
    packet.finalize(true);
    user->Send(packet);
    return 1;
