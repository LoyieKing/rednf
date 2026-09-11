// ---------------------------------------------------------------------------
// make_blood_dungeon_info_blank —— ORIG T 0x0859e480
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info_blank.md
// ---------------------------------------------------------------------------
void CParty::make_blood_dungeon_info_blank(PacketGuard& packet)
{
    packet.put_byte(get_member_count());
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(m_member[i].m_pUser->get_unique_id() & 0xffff);
            packet.put_byte(0);
        }
    }
    packet.put_int(0);
}