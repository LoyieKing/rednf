void CUser::SendRandomBoxStatistic(int boxKind, ENUM_RANDOMBOX_ACTION_KIND actionKind)
{
    // ORIG 0x8676826：无 proxy 判空/无具名槽（call 结果 %eax 直传 (%esp)，&pkt 先
    // 载 %ebx 跨 call，技巧 37），SendPacket 无条件调（非虚 null-this 安全）。
    Packet_Randombox_statistic pkt;
    pkt.m_boxKind = static_cast<char>(boxKind);
    pkt.m_actionKind = static_cast<char>(actionKind);
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0xc);
