int TimerClearDungeonScoreScoll::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // [b23S3C] P48 去缓存：get_user 每调用点重取×3；fatigue/max 结果寄存器直比；
    // 判定链拆早返岛（fresh .o 134/134 零差）。
    // ORIG 0x086345f2：GetParty(p1) + check_timer_key(0x12, p2)（用 p2 非 p3）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x12, p2))
        return 0;
    if (party->get_dungeon_clear_state() == 0x03)
    {
        PacketGuard packet;
        packet.put_header(1, 0x49);
        packet.put_byte(1);
        for (int i = 0; i <= 3; ++i)
        {
            // ORIG 0x08634696：get_user ×3（每次调用点重取，P183 去缓存）。
            if (party->get_user(i) == 0)
                packet.put_short(-1);
            else
            {
                if (party->get_user(i)->getCurCharacTotalFatigue() ==
                    party->get_user(i)->getCurCharacTotalMaxFatigue())
                    packet.put_short(0);
                else
                    packet.put_short(1);
            }
        }
        packet.finalize(true);
        party->send_to_party(packet);
        party->set_dungeon_clear_state(0x04);
        party->insert_time_dungeon_score_scroll_msg(0x1e);
        party->insert_time_card_select_msg(0x04);
    }
    return 1;
