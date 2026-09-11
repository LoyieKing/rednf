// ORIG execute 0x084ab94c：GM 校验后向 monitor 发 Packet_Monitor_Event_End。
void CEndEvent::execute()
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }
    Packet_Monitor_Event_End packet;
    packet.m_eventCode = static_cast<unsigned int>(m_eventCode);
    // ORIG：GetServerProxy 结果 eax 直通 receiver（无 proxy 具名槽）；packet 基址
    // 跨 call 物化 %ebx 为 4.1 调度产物。N96/P0：register asm 绑定全域禁用，
    // 保持自然表达式直传（残差记 N18a/N7 族）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0xe);
}