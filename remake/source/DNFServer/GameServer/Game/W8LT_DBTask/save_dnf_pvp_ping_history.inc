bool DB_GatheringParyStatus::save_dnf_pvp_ping_history(SIG_PARTY_STATUS* sig)
{
    char buf[8][0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    int i;
    for (i = 0; i <= 7; ++i)
        sprintf(buf[i], "%d.%d.%d.%d",
                (unsigned char)sig->m_partyEntries[i][9],
                (unsigned char)sig->m_partyEntries[i][10],
                (unsigned char)sig->m_partyEntries[i][11],
                (unsigned char)sig->m_partyEntries[i][12]);
    db->set_query(
        "inSert into dnf_party_ping_history_pvp(str_ip,m_id,nat_type_1,ping1,mtu_1,"
        "nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,"
        "nat_type_5,ping5,mtu_5,nat_type_6,ping6,mtu_6,nat_type_7,ping7,mtu_7,"
        "nat_type_8,ping8,mtu_8,occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4,"
        "ip_5,ip_6,ip_7,ip_8, end_reason) values('%s',%s,%d,%d,%d,%d,%d,%d,%d,%d,"
        "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s',"
        "'%s','%s','%s','%s','%s',%d)",
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
        ((DB_PartyEntriesView*)sig)->m_nat4,
        ((DB_PartyEntriesView*)sig)->m_ping4,
        ((DB_PartyEntriesView*)sig)->m_mtu4,
        ((DB_PartyEntriesView*)sig)->m_nat5,
        ((DB_PartyEntriesView*)sig)->m_ping5,
        ((DB_PartyEntriesView*)sig)->m_mtu5,
        ((DB_PartyEntriesView*)sig)->m_nat6,
        ((DB_PartyEntriesView*)sig)->m_ping6,
        ((DB_PartyEntriesView*)sig)->m_mtu6,
        ((DB_PartyEntriesView*)sig)->m_nat7,
        ((DB_PartyEntriesView*)sig)->m_ping7,
        ((DB_PartyEntriesView*)sig)->m_mtu7,
        sig->m_field18,
        sig->m_field1c,
        buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
        (unsigned char)sig->m_field20);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x58db, 5);
        trace("DB_GatheringParyStatus failed");
    }
    return 1;
