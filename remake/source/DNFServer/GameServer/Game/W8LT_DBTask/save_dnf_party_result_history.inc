bool DB_GatheringParyStatus::save_dnf_party_result_history(SIG_PARTY_STATUS* sig, int idx)
{
    char buf[0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    // 同 pvp 版：N18a 残差 2 对（装载序+折叠），行为等价。
    sprintf(buf, "%d.%d.%d.%d",
            (unsigned char)sig->m_tryEntries[idx][2],
            (unsigned char)sig->m_tryEntries[idx][3],
            (unsigned char)sig->m_tryEntries[idx][4],
            (unsigned char)sig->m_tryEntries[idx][5]);
    db->set_query(
        "inSert into dnf_party_result_history (str_ip,nat_type,m_id,party_result,"
        "occ_time,map_id,dungeon_id,peer_nat_type,peer_ip,established_time, "
        "IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, "
        "relay_tcp_port) values('%s',%d,%s,%d,now(),%d,%d,%d,'%s',%d, %d, %d, "
        "'%s', %d, %d)",
        sig->m_ip,
        sig->m_natType,
        NumberToString(sig->m_accId, 0),
        (unsigned char)sig->m_tryEntries[idx][0],
        sig->m_field18,
        sig->m_field1c,
        (unsigned char)sig->m_tryEntries[idx][1],
        buf,
        reinterpret_cast<const DB_TryNatView*>(
            reinterpret_cast<const char*>(
                sig) + idx * 10)->m_sec.m_f,
        (unsigned char)sig->m_field21,
        sig->m_field22,
        G_CEnvironment()->m_serverEnvir.m_relayIP,
        G_CEnvironment()->m_serverEnvir.m_relayUDPPort,
        G_CEnvironment()->m_serverEnvir.m_relayTCPPort);
    ok = db->exec(true);
    if (!ok)
    {
        // ORIG：buf 远槽/trace 近槽 ⇒ 临时链式翻槽序（技巧3，同 pvp 版）。
        cMyTrace(__PRETTY_FUNCTION__, 0x590f, 5)(
            "DB_GatheringParyStatus failed");
    }
    return 1;
