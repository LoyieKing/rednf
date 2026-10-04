// ORIG 0x8420e1a：PVP 对战结果。idx 选择 m_tryEntries[idx]（10 字节行）：
// [0]/[1] byte（party_result/dungeon_id）、[2..5] byte 拼 peer_ip、
// [6..9] int（peer_nat_type）。relay_* 取 G_CEnvironment()->m_serverEnvir。
// [b28c-L19 SIGFIX] 返回 int→bool；残差 N76/N18a（装载序 ecx/edx + 常量 0x20 折叠位置）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08420e1a
bool DB_GatheringParyStatus::save_dnf_pvp_result_history(SIG_PARTY_STATUS* sig, int idx)
{
    char buf[0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    // ORIG 0x8421…：`lea (%ecx,%eax);add $0x20;mov 0xe(%eax)` 三段：
    // 4.4 对语句内联合表达式恒折叠单 add（两步拆分则指针落槽多 2 对），
    // 装载序（idx 先/sig 后）不可控 → N18a 残差 2 对，行为等价。
    sprintf(buf, "%d.%d.%d.%d",
            sig->m_tryEntries[idx].m_peerIp[0],
            sig->m_tryEntries[idx].m_peerIp[1],
            sig->m_tryEntries[idx].m_peerIp[2],
            sig->m_tryEntries[idx].m_peerIp[3]);
    db->set_query(
        "inSert into dnf_party_result_history_pvp (str_ip,nat_type,m_id,party_result,"
        "occ_time,map_id,dungeon_id,peer_nat_type,peer_ip, established_time, "
        "IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, "
        "relay_tcp_port ) values('%s',%d,%s,%d,now(),%d,%d,%d,'%s',%d, %d, %d, "
        "'%s', %d, %d)",
        sig->m_ip,
        sig->m_natType,
        NumberToString(sig->m_accId, 0),
        sig->m_tryEntries[idx].m_result,
        sig->m_field18,
        sig->m_field1c,
        sig->m_tryEntries[idx].m_peerNat,
        buf,
        // [r3-W8LT_DBTask] 已改：`(char*)sig + idx*10 + 0x20` 显式算术 +
        // DB_TryNatSection 单级成员 ⇒ 复现 ORIG 0x08420e1a `add $0x20,%eax;
        // mov 0xe(%eax)`（gap 1→0、ndiff 5→4、差集归零）。残余 = 装载序/常量
        // 结合顺序（N18a）。反例已记：改 `&sig->m_tryEntries[idx][0]` 反劣
        // （174→175、add:3/lea:1）。
        // [M8DBB 2026-09-11 负结论] 常量前置形 `sig + 0x20 + idx*10` 实测
        // **零变化**（174/175、diff blocks 17 全同，仅 `mov 0xc(%ebp),%ecx`
        // 与 `add $0x20` 的发射序不变）⇒ 回滚。残余归 N18a 结合序封顶。
        sig->m_tryEntries[idx].m_estTime,
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
}