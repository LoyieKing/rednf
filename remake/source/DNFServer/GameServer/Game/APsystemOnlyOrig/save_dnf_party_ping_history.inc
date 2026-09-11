// ORIG 0x84217b6：4 名成员 ping 上报（结构同 pvp 版，仅 4 组 + dnf_party_ping_history）。
// [b28c-L19 SIGFIX] 返回 int→bool（同上）。
bool DB_GatheringParyStatus::save_dnf_party_ping_history(SIG_PARTY_STATUS* sig)
{
    char buf[4][0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    int i;
    for (i = 0; i <= 3; ++i)
        sprintf(buf[i], "%d.%d.%d.%d",
                (unsigned char)sig->m_partyEntries[i][9],
                (unsigned char)sig->m_partyEntries[i][10],
                (unsigned char)sig->m_partyEntries[i][11],
                (unsigned char)sig->m_partyEntries[i][12]);
    db->set_query(
        "inSert into dnf_party_ping_history(str_ip,m_id,nat_type_1,ping1,mtu_1,"
        "nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,"
        "occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4, end_reason) values('%s',"
        "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s','%s',%d)",
        sig->m_ip,
        NumberToString(sig->m_accId, 0),
        ((DB_PartyEntriesView*)sig)->m_nat0,
        ((DB_PartyEntriesView*)sig)->m_ping0,
        ((DB_PartyEntriesView*)sig)->m_mtu0,
        ((DB_PartyEntriesView*)sig)->m_nat1,
        ((DB_PartyEntriesView*)sig)->m_ping1,
        ((DB_PartyEntriesView*)sig)->m_mtu1,
        ((DB_PartyEntriesView*)sig)->m_nat2,
        ((DB_PartyEntriesView*)sig)->m_ping2,
        ((DB_PartyEntriesView*)sig)->m_mtu2,
        ((DB_PartyEntriesView*)sig)->m_nat3,
        ((DB_PartyEntriesView*)sig)->m_ping3,
        ((DB_PartyEntriesView*)sig)->m_mtu3,
        sig->m_field18,
        sig->m_field1c,
        buf[0], buf[1], buf[2], buf[3],
        (unsigned char)sig->m_field20);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x5964, 5);
        trace("DB_GatheringParyStatus failed");
    }
    return 1;
}