int user_creature::CCreatureMgr::SetStomach(int stomachValue)
{
    if (m_equippedCreature != 0)
    {
        m_equippedCreature->SetStomach(stomachValue);
        CPacketRespondent pkt(m_user, 0, 0);
        // ORIG 0x08339ee0 求值序：ctor 后先 GetStomach()→%ebx，再
        // CCreatureItem::GetUid() call（+0x69，W 0x833efbe 外联）→参数 (uid,stomach)。
        pkt.MakeNotipacketCreatureState(
            reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid(),
            m_equippedCreature->GetStomach());
        pkt.Send();
    }
    return 1;
}