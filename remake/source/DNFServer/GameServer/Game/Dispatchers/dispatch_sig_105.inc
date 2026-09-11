// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ff344 orig/ours=117/120 ndiff-blocks=22
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | movswl %ax,%ebx | mov    %ebx,0x4(%esp)
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
    while (party == 0)
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
}