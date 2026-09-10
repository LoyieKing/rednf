void CUser::SendRestrictedGoods(bool flag, int itemIdx)
{
    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(0, 0x44);
    if (flag)
    {
        pkt->put_byte(0);
        pkt->put_short(1);
        pkt->put_int(itemIdx);
    }
    else
    {
        pkt->put_byte(1);
        pkt->put_short((int)m_list711ec.size());
        // b28d-solo-CUser p167：同 IsRestrictedGoods，ORIG 用非 const `_List_iterator<int>`。
        for (std::list<int>::iterator it = m_list711ec.begin();
             it != m_list711ec.end(); ++it)
        {
            pkt->put_int(*it);
        }
    }
    pkt->finalize(true);
    Send(packet);
