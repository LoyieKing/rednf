void CMonitorServerProxy::SendMemberPayTax(bool payTax, unsigned int charNo,
                                           int money, int fatigue)
{
    if (payTax == false)
    {
        return;
    }
    if (money == 0 && fatigue == 0)
    {
        return;
    }
    Packet_Monitor_Member_Pay_Tax pkt;
    pkt.m_charNo = charNo;
    pkt.m_money = money;
    pkt.m_fatigue = fatigue;
    SendPacket((char*)&pkt, pkt.packetSize);
