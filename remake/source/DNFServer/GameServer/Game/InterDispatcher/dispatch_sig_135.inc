// [b28c-31] AE：两守卫改正向早退形（P16/P59），86=86 仅剩重定位显示差。
int Inter_NoticePowerWarInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarInfoPktView* pPkt = reinterpret_cast<Inter_NoticePowerWarInfoPktView*>(data);
    GlobalData::s_power_manager->SetPowerInfo(
        (char)pPkt->m_12,
        pPkt->m_a,
        pPkt->m_e);

    // [b28c-31] ORIG 0x084d4e70/0x084d4e90：两守卫均为正向早退
    // （`test %al,%al; je <继续>` / `cmp $1;je <true>; cmp $2;jne <false>`），
    // 4.4 对 `if(!f()){...}` 嵌套产 xor$1 反形 ⇒ 改写正向 return 0 形（P16/P59）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 0x84d4e58：G_CEnvironment() 短路双调（O0 不 CSE）。==1||==2 → 不广播。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    PacketGuard packet;
    packet.put_header(0, 0xbc);
    packet.put_byte((int)pPkt->m_12);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}