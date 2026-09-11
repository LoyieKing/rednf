bool Timer_TournamentDugeonClearStateNotiReward::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // [b23S3C] check_timer_key 第二实参为 p2（非 p3，ORIG 0x0863b318）；P48 去缓存
    // get_user 每调用点重取×3（fresh .o 133/133 零差）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2e, p2))  // ORIG 0x0863b318：用 p2（0x10(%ebp)），非 p3
        return 0;
    if (party->GetTournamentDungeonClearState() == 0x01)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1ca);
        packet.put_byte(1);
        for (int i = 0; i <= 3; ++i)
        {
            if (party->get_user(i) == 0)
            {
                packet.put_byte(-1);
            }
            else
            {
                // ORIG 0x0863b40a：get_user ×3（每次调用点重取，P183 去缓存）；
                // 结果寄存器直比（sete）。
                if (party->get_user(i)->getCurCharacTotalFatigue() ==
                    party->get_user(i)->getCurCharacTotalMaxFatigue())
                    packet.put_byte(0);
                else
                    packet.put_byte(1);
            }
        }
        packet.finalize(true);
        party->send_to_party(packet);
        party->SetTournamentDungeonClearState(0x02);
        party->InsertTournamentDungeonTimer((TIMER_MESSAGE)0x2f);
    }
    return 1;
}