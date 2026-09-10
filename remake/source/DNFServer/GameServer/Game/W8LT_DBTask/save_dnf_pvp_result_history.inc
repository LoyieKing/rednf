bool DB_GatheringParyStatus::save_dnf_pvp_result_history(SIG_PARTY_STATUS* sig, int idx)
{
    char buf[0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    // ORIG 0x8421…：`lea (%ecx,%eax);add $0x20;mov 0xe(%eax)` 三段：
    // 4.4 对语句内联合表达式恒折叠单 add（两步拆分则指针落槽多 2 对），
    // 装载序（idx 先/sig 后）不可控 → N18a 残差 2 对，行为等价。
    sprintf(buf, "%d.%d.%d.%d",
            (unsigned char)sig->m_tryEntries[idx][2],
            (unsigned char)sig->m_tryEntries[idx][3],
            (unsigned char)sig->m_tryEntries[idx][4],
            (unsigned char)sig->m_tryEntries[idx][5]);
    db->set_query(
        "inSert into dnf_party_result_history_pvp (str_ip,nat_type,m_id,party_result,"
        "occ_time,map_id,dungeon_id,peer_nat_type,peer_ip, established_time, "
        "IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, "
        "relay_tcp_port ) values('%s',%d,%s,%d,now(),%d,%d,%d,'%s',%d, %d, %d, "
        "'%s', %d, %d)",
        sig->m_ip,
        sig->m_natType,
        NumberToString(sig->m_accId, 0),
        (unsigned char)sig->m_tryEntries[idx][0],
        sig->m_field18,
        sig->m_field1c,
        (unsigned char)sig->m_tryEntries[idx][1],
        buf,
        // [b28d-solo-W8LT_DBTask] P214① 探针复核：数组名算术产 sig+0x28 三段
        // （多 3 条）且仍折 0x6 进 disp，ORIG 的 add $0x20/$0xe 拆分不可复现
        // ⇒ 维持视图表达式，N4/N18a 封顶。
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
        // ORIG 0x8421…：buf 占远槽 -0x40、cMyTrace 临时槽 -0x30（近）。
        // 4.4 具名块对象把 buf 挤到 -0x30 ⇒ 改临时链式（技巧3）翻槽序。
        cMyTrace(__PRETTY_FUNCTION__, 0x5864, 5)(
            "DB_GatheringParyStatus failed");
    }
    return 1;
