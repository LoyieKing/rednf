// ORIG 0x842156a：普通队伍对战结果（结构同 pvp 版，表 dnf_party_result_history）。
// [b28c-L19 SIGFIX] 返回 int→bool；残差同 pvp 版（N76/N18a）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0842156a
bool DB_GatheringParyStatus::save_dnf_party_result_history(SIG_PARTY_STATUS* sig, int idx)
{
    char buf[0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    // [r3] 同 pvp 版：`+0x20` 显式算术（ORIG 0842156a `add $0x20,%eax;
    // mov 0xe(%eax)`）⇒ gap 1→0、ndiff 5→4；残余 = 装载序/常量结合序（N18a）。
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
        reinterpret_cast<const DB_TryNatSection*>(
            reinterpret_cast<const char*>(
                sig) + idx * 10 + 0x20)->m_f,
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
}