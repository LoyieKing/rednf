void Dispatcher_TournamentRewardSelectState::send(CUser* user, ParamBase& param)
{
    // [b23S3C] ORIG 0x081dfd74：param 引用槽(-0x14) put_header 后回读判定（P0 无值槽，
    // 去 m_4 hoist；PacketTypes.h 冻结故用局部 overlay）；get_user 每用点重取
    // （ORIG 3 次 call），==0 call 结果直比出 test;sete;test;je（P197④）；f1 升格
    // %ebx + `cmp %ax,%bx`（P197②/P200）。GetPartyIndex movswl%ax,%ebx 已同形保留。
    struct TRSS { ParamBase hdr; int m_4; };
    TRSS& out = *reinterpret_cast<TRSS*>(&param);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x1ca);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        CParty* party = G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
        if (party != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (party->get_user(i) == 0)
                {
                    packet.put_byte(-1);
                }
                else if (party->get_user(i)->getCurCharacTotalFatigue() ==
                         party->get_user(i)->getCurCharacTotalMaxFatigue())
                {
                    packet.put_byte(0);
                }
                else
                {
                    packet.put_byte(1);
                }
            }
            packet.finalize(true);
            party->send_to_party(packet);
            party->SetTournamentDungeonClearState((char)0x02);
            party->InsertTournamentDungeonTimer((TIMER_MESSAGE)0x2f);
        }
    }
