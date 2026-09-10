int TimerStartRelayBattle::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x0863640c：GetPvp(p1, 0, 99) → relay(=room+0x620) 计时键比对(p2) → OnFight。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 99);
    if (room == 0)
    {
        return 0;
    }
    if (room->m_relayMgr.GetTimerKey() != p2)   // ORIG room+0x620
    {
        return 0;
    }
    room->m_relayMgr.OnFight();
    return 1;
