void CRelayBattleMgr::OnRequestFight(int seat)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    // [b29 P248档②] 字段 bool 化后裸形即复现 ORIG `movzbl; xor$1; test`（P0 合规）。
    if (!m_battles[seat].m_requestFight)
    {
        m_battles[seat].m_requestFight = 1;
        IncreaseChangeSequenceCount(m_battles[seat].m_team);
    }
    else
    {
        m_battles[seat].m_requestFight = 0;
        DecreaseChangeSequenceCount(m_battles[seat].m_team);
    }
    SendRequestFight(seat);
