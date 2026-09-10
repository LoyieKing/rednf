int TimerClearDungeonReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x11, p2))
    {
        return 0;
    }
    if (party->get_dungeon_clear_state() == 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x48);
        packet.put_byte(1);
        packet.finalize(true);
        party->send_to_party(packet);
        party->set_dungeon_clear_state(3);
        party->insert_time_dungeon_score_scroll_msg(0x1e);
    }
    return 1;
