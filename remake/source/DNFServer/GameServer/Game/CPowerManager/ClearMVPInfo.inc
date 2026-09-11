// [b28d-solo-CPowerManager] IDENTICAL(186=186, ndiff 0) | ORIG 0x0847f5de | clean
// 全等（P227 全表达式临时链复现两 trace 槽序 + varargs 跨 ctor 保活）；P139 恢复
// `_ZZN13CPowerManager12ClearMVPInfoEvE19__PRETTY_FUNCTION__` 局部静态符号。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
void CPowerManager::ClearMVPInfo()
{
    // [b28d-28] ORIG 0x0847f5fe/-0x38 与 0x0847f83x/-0x28：两个 cMyTrace 槽
    // 深→浅（4.1 方向）；具名对象 4.4 恒反向 ⇒ 用全表达式临时链（P227/P221）
    // 复现槽序，同时使 `tr(...)` 的 varargs（G_CEnvironment()->+0x378）在
    // ctor 前求值并以 %ebx 跨 call 保活（ORIG 0x0847f84x `mov 0x378(%eax),%ebx`）。
    // [b28d-solo] P139：ORIG file 串为 `_ZZN13CPowerManager12ClearMVPInfoEvE19
    // __PRETTY_FUNCTION__`（0x08c72f40）⇒ 传 __PRETTY_FUNCTION__ 恢复局部静态符号。
    cMyTrace(__PRETTY_FUNCTION__, 0x143, 0)("CLEAR MVP INFO");
    memset(m_rankings, 0, sizeof(m_rankings));
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x149), true);
    **guard << 0x1a5;
    **guard << -1;
    // ORIG 直读 G_CEnvironment()+0x378（m_serverEnvir.m_serverGroup），两次
    // 各自重物化（P183）；禁 getter 调用（EXTRA symbol）与跨调用缓存。
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    cMyTrace(__PRETTY_FUNCTION__, 0x152, 0)
        ("PUT DB MSG - E_DELETE_POWER_WAR_STATUE_MESSAGE / server_group : %d",
         G_CEnvironment()->m_serverEnvir.m_serverGroup);

    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        mgr->DeleteSpecificMessage(1);
        mgr->DeleteSpecificMessage(2);
        mgr->DeleteSpecificMessage(3);
    }

    PacketGuard pkt;
    pkt.put_header(0, 0xc0);
    pkt.put_byte(2);
    pkt.put_byte(3);
    char* msg = (char*)"null string";   // ORIG char* → put_str(char*,int) 重载
    int len = static_cast<int>(strlen(msg));
    for (int i = 0; i < 3; ++i)
    {
        pkt.put_byte(i + 1);
        pkt.put_int(len);
        pkt.put_str(msg, len);
        pkt.put_int(len);
        pkt.put_str(msg, len);
    }

    pkt.finalize(true);
    G_GameWorld()->send_all_with_state(pkt, 3);
}