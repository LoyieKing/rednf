int Dispatcher_Card_Select_Right_State::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b23S3C] ORIG 0x081ff504：PF 直写（__PRETTY_FUNCTION__ static 局多 guard 记账开销）；
    // GetPartyIndex 实参直传 movswl 升格 %ebx（去 partyIdx 槽，P200）；member 无槽、
    // get_user 每用点重取（ORIG 3 call）、fatigue 对 ebx 缓存 + `cmp %ax,%bx`
    // （TournamentRewardSelectState::send 同 TU 已闭合孪生形）。
    if (user->get_state() != 5)
        return 0;
    if (user->getMoveSpace() == 1)
        return 0;
    CParty* party = G_CGameManager()->GetParty(
        (int)(short)user->GetPartyIndex());
    // [b28d-solo-Dispatchers] P295：ORIG 0x081ff5xx `cmpl $0x0,-0x10(%ebp); sete %al;
    //   test %al,%al; je <跳过失败路径>`（4.1 sete 物化）⇒ `while (p == 0) { return …; }`
    //   复现（普通 if 形 4.4 恒折 `jne` 直跳，探针族 P295/P225）。
    while (party == 0)
    {
        return LineFunc(0x9328, __PRETTY_FUNCTION__, 0, 0);
    }
    if (party->m_battleField.m_pDungeon != 0 && party->m_battleField.m_pDungeon->isTowerOfDespairDungeon())
        return 0;
    if (!party->CheckRecvFromAllMember(user))
        return 0;
    if (party->get_dungeon_clear_state() > 3)   // cmp $0x3 / setg
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x197, 1, 0, 0);
        return 0;
    }
    PacketGuard guard;
    guard.put_header(1, 0x49);
    guard.put_byte(1);
    for (int i = 0; i < 4; ++i)   // cmpl $0x3 / setle
    {
        if (party->get_user(i) == 0)
        {
            guard.put_short(-1);
        }
        else if (party->get_user(i)->getCurCharacTotalFatigue() ==
                 party->get_user(i)->getCurCharacTotalMaxFatigue())
        {
            guard.put_short(0);
        }
        else
        {
            guard.put_short(1);
        }
    }
    guard.finalize(true);
    party->send_to_party(guard);
    party->set_dungeon_clear_state(4);
    party->insert_time_card_select_right_msg(0x1e);
    party->insert_time_card_select_msg(4);
    return 0;
