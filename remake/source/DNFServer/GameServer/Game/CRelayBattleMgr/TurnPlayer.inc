int CRelayBattleMgr::TurnPlayer()
{
    if (m_pRoom->IsEndPvpBattle())
    {
        return 1;
    }
    PacketGuard guard;
    int index;   // ORIG -0x28（比 guard -0x24 深；4.4 把聚合放最深 ⇒ NEAR 槽互换，N82附）
    int seat;
    int team;
    int count;
    int i;
    guard.put_header(0, 0x70);
    guard.put_byte(2);
    for (team = 1; team < 3; ++team)
    {
        seat = GetNextPlayer(team);
        guard.put_byte(seat);
        index = guard.get_index();
        count = 0;
        guard.put_byte(count);   // ORIG 0x085deb71/0x085deb74：读 count 槽（非立即数）
        if (m_changeSeqCount[team] > 0)
        {
            for (i = 0; i < 8; ++i)
            {
                if (m_battles[i].m_requestFight &&
                    m_battles[i].m_team == team)
                {
                    guard.put_byte(i);
                    guard.put_short(m_battles[i].m_seq);
                    ++count;
                    m_battles[i].m_requestFight = 0;
                }
            }
        }
        guard.put_byte(index, count);
    }
    guard.finalize(true);
    m_pRoom->send_to_pvp(guard);
    ResetChangeSequenceCount();
    return 1;
