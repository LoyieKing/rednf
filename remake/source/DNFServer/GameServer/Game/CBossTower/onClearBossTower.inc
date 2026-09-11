// ----------------------------------------------------------------------------
// ORIG T 0x08144508 _ZN8WongWork10CBossTower16onClearBossTowerEv：
// partyType==1 → 仅发包(0,0xfe)；否则 generateRewardExp/generateRewardItem/
// SendRewardPacket/InsertRewardExp/InsertRewardItems 任一非 1 → 0；
// SendELPLPacket()==1 时 SetEPLPState(2) 并 InsertTimer(OBJ 7, m_idx, msg 0xa4,
// 20s, key 0xa4, 0xa4)。返回结果标志。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144508：槽平移 4 + bool 物化（N82/P197④）。
// [b28d-solo-CBossTower] AE：删 party 局部 + 6 处 `!=1` 改 `!f()`（本类
//   generateRewardExp/generateRewardItem/SendRewardPacket/InsertRewardExp/
//   InsertRewardItems/SendELPLPacket bool 化，ORIG 调用点全 xor）⇒ DIFF→AE。
int CBossTower::onClearBossTower()
{
    if (m_party->m_battleField.m_bloodState == 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0xfe);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        if (!generateRewardExp())
            return 0;
        if (!generateRewardItem())
            return 0;
        if (!SendRewardPacket())
            return 0;
        if (!InsertRewardExp())
            return 0;
        if (!InsertRewardItems())
            return 0;
    }

    if (!SendELPLPacket())
        return 0;

    m_party->SetEPLPState(2);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa4, 0x14, 0xa4, 0xa4);
    return 1;
}