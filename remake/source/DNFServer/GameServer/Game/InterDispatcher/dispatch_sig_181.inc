int Inter_PowerWarEndInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PowerWarEndInfoPktView* pPkt = reinterpret_cast<Inter_PowerWarEndInfoPktView*>(data);

    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0xdf);
    packet.put_byte((unsigned int)pPkt->m_12);
    packet.put_int(pPkt->m_13);
    packet.put_int(pPkt->m_17);
    packet.put_int(pPkt->m_1b);
    packet.put_int(pPkt->m_1f);
    packet.put_int(pPkt->m_23);

    if (GlobalData::s_power_manager->GetWinnerSide() == user->getPowerSide())
    {   // 与 ORIG +0xa6a4 对应（CDataManager 胜方附加字段，暂无具名成员）
        packet.put_int(G_CDataManager()->m_powerParameter.m_basicPowerWarPoint);
    }
    else
    {
        packet.put_int(0);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}