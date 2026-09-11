// ORIG SendCreatureItemList @ 0x833bdd2（41 指令，SendCreatureItemList.md）：
// CPacketRespondent(m_user,0,0) → MakeNotipacketCreatureItemList(&m_creatureItems,-1)
// → Send()（透传 CUser::Send 返回值）→ 析构。
bool CCreatureMgr::SendCreatureItemList() const
{
    CPacketRespondent pkt(m_user, 0, 0);
    pkt.MakeNotipacketCreatureItemList(&m_creatureItems, -1);
    return pkt.Send();
}
