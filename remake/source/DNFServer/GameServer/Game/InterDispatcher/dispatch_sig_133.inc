int Inter_NoticePowerWarStartTime::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarStartTimePktView* pPkt = reinterpret_cast<Inter_NoticePowerWarStartTimePktView*>(data);

    // CEnvironment+0x1a8 环境槽未在权威头建模（沿 CUserOnlyOrig.cpp:9027 先例，
    // §4.3 例外保留地址形式；ORIG 两次独立 G_CEnvironment 调用，P48）
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    GlobalData::s_power_manager->SetPowerWarStartTime(
        (unsigned char)pPkt->m_year,
        (unsigned char)pPkt->m_month,
        (unsigned char)pPkt->m_day,
        (unsigned char)pPkt->m_hour);

    PacketGuard packet;
    packet.put_header(0, 0xa1);
    packet.put_byte((unsigned int)pPkt->m_year);
    packet.put_byte((unsigned int)pPkt->m_month);
    packet.put_byte((unsigned int)pPkt->m_day);
    packet.put_byte((unsigned int)pPkt->m_hour);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}