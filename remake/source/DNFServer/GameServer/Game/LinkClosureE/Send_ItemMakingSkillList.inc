// ORIG T _ZNK16CItemMakingSkill24Send_ItemMakingSkillListEP5CUser @0x08545a62。
// 报告 docs/class_func_reports/CItemMakingSkill/Send_ItemMakingSkillList.md。
// put_header(0,0x15e) → put_byte(0xb) → i=0..0xa {put_byte(i); put_short(表[i])}
// → finalize(true) → Send → 返回 1。
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
}