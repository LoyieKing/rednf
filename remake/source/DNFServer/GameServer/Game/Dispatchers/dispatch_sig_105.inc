int Dispatcher_Score_Scroll_State::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() != 5)
    {
        return 0;
    }
    // ORIG 0x0822..：`call G_CGameManager; call GetPartyIndex; movswl %ax,%ebx;
    //   mov %ebx,0x4(%esp)` —— partyIdx/gameMgr 均无栈槽（我方具名使帧 0x30→0x40）。
    CParty* party = G_CGameManager()->GetParty((int)user->GetPartyIndex());
    if (party == 0)
    {
        return LineFunc(0x92fa,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!party->CheckRecvFromAllMember(user))
    {
        return 0;
    }
    if ((int)party->get_dungeon_clear_state() > 2)
    {
        // ORIG：getHackAnalyzer() 结果经 %eax 直传（无 analyzer 具名指针槽）。
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x197, 1, 0, 0);
        return 0;
    }
    PacketGuard response;
    response.clear();
    response.put_header(1, 0x48);
    response.put_byte(1);
    response.finalize(true);
    party->send_to_party(response);
    party->set_dungeon_clear_state((char)3);
    party->insert_time_dungeon_score_scroll_msg(0x1e);
    return 0;
