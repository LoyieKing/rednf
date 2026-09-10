int Timer_QuickPartyWaitingStartGame::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;

    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;

    party->send_quick_party_matching_result(false, (CUser*)0);
    party->set_quick_party(false);

    // [b28d-solo-TimerDispatcher2] ORIG `call getManager; mov %eax,(%esp); call
    // getCurCharacName; mov %eax,%ebx`（无 managerInfo 槽、结果落 %ebx 跨 cMyTrace
    // ctor）⇒ P52 链式内联 + P231 纯 register 提示。
    register const char* managerName = party->getManager()->getCurCharacName();
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x16a8, 0);
        trace("QUICK_PARTY_LOG : PARTY DESTROY AND LEAVE DUNGEON !! : MANAGER (%s)", managerName);
    }

    for (int i = 0; i < 4; ++i)   // asm: cmp $3 / setle（i <= 3）
    {
        if (!party->checkValidUser(i))
            continue;

        CUser* user = party->get_user(i);
        const int ret = party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
        if (ret != 1)
        {
            G_GameWorld()->out_from_dungeon(user);
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(1);
            user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
            packet.finalize(true);
            G_GameWorld()->send_all(packet, user);
        }
    }
    return 1;
