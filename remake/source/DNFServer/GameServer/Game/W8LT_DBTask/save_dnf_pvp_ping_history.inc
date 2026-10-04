// ORIG 0x8421066：8 名成员 ping 上报。每行 m_partyEntries[i][9..12] 拼
// "a.b.c.d" 存入 buf[i]（0x10 字节行），SQL 25 个占位 + now()。
// [b28c-L19 SIGFIX] 返回 int→bool（ORIG __PRETTY_FUNCTION__ 串实证）。
bool DB_GatheringParyStatus::save_dnf_pvp_ping_history(SIG_PARTY_STATUS* sig)
{
    char buf[8][0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    int i;
    for (i = 0; i <= 7; ++i)
        sprintf(buf[i], "%d.%d.%d.%d",
                (unsigned char)sig->m_partyEntries[i].m_ip[0],
                (unsigned char)sig->m_partyEntries[i].m_ip[1],
                (unsigned char)sig->m_partyEntries[i].m_ip[2],
                (unsigned char)sig->m_partyEntries[i].m_ip[3]);
    db->set_query(
        "inSert into dnf_party_ping_history_pvp(str_ip,m_id,nat_type_1,ping1,mtu_1,"
        "nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,"
        "nat_type_5,ping5,mtu_5,nat_type_6,ping6,mtu_6,nat_type_7,ping7,mtu_7,"
        "nat_type_8,ping8,mtu_8,occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4,"
        "ip_5,ip_6,ip_7,ip_8, end_reason) values('%s',%s,%d,%d,%d,%d,%d,%d,%d,%d,"
        "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s',"
        "'%s','%s','%s','%s','%s',%d)",
        sig->m_ip,
        NumberToString(sig->m_accId, 0),
        sig->m_partyEntries[0].m_nat,
        sig->m_partyEntries[0].m_ping,
        sig->m_partyEntries[0].m_mtu,
        sig->m_partyEntries[1].m_nat,
        sig->m_partyEntries[1].m_ping,
        sig->m_partyEntries[1].m_mtu,
        sig->m_partyEntries[2].m_nat,
        sig->m_partyEntries[2].m_ping,
        sig->m_partyEntries[2].m_mtu,
        sig->m_partyEntries[3].m_nat,
        sig->m_partyEntries[3].m_ping,
        sig->m_partyEntries[3].m_mtu,
        sig->m_partyEntries[4].m_nat,
        sig->m_partyEntries[4].m_ping,
        sig->m_partyEntries[4].m_mtu,
        sig->m_partyEntries[5].m_nat,
        sig->m_partyEntries[5].m_ping,
        sig->m_partyEntries[5].m_mtu,
        sig->m_partyEntries[6].m_nat,
        sig->m_partyEntries[6].m_ping,
        sig->m_partyEntries[6].m_mtu,
        sig->m_partyEntries[7].m_nat,
        sig->m_partyEntries[7].m_ping,
        sig->m_partyEntries[7].m_mtu,
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
}