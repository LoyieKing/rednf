bool Timer_TournametDungeonClearStateSelect::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    (void)p3;   // ORIG 0x0863b68c：check_timer_key 用 p2（0x10(%ebp)），非 p3
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2f, p2))
        return 0;
    if (party->GetTournamentDungeonClearState() == 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1cb);
        packet.put_byte(1);
        CTournamentDungeonReward& reward =
            *static_cast<CTournamentDungeonReward*>(   // ORIG 0x0863b71b：mov party; add $0x17bc,%eax 单级
                (void*)((unsigned long)party + 0x17bcUL));
        std::vector<int> members;
        party->GetValidPartyMember(members);
        const bool victory = party->isTournamentVictory();
        if (!reward.isCardSelectComplete((ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members))
            reward.ForceSelectCard(members, victory);
        reward.MakeSelectPacket(&packet);
        packet.finalize(true);
        party->send_to_party(packet);
        party->SetTournamentDungeonClearState(3);
        party->TournamentDungeonClear(victory);
    }
    return 1;
}