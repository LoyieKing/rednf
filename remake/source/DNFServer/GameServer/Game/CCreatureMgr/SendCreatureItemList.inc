bool CCreatureMgr::SendCreatureItemList() const
{
    CPacketRespondent pkt(m_user, 0, 0);
    pkt.MakeNotipacketCreatureItemList(&m_creatureItems, -1);
    return pkt.Send();
}
