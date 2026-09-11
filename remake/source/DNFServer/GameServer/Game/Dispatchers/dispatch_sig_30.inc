// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0820559a orig/ours=137/139 ndiff-blocks=9
//   帧差 sub $30 vs $20 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0x10(%ebp) | cmpl   $0x0,-0x10(%ebp)
int Dispatcher_Back2Village::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() == 0xd)
    {
        advancealtar::Manager::leaveUser(user);
        return 0;
    }
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = (CParty*)user->GetParty();
    // [DispG1/30] P133：ORIG 0x082055f9 两条件各自 jne 直跳失败岛 ⇒ 拆独立 if
    if (party == 0)
    {
        return 0;
    }
    if (party->GetEPLPState() != 0)
    {
        return 0;
    }
    // [DispG1/30] P59：ORIG 0x08205632 `setne + je 0x08205749`（!= 分支 fallthrough）
    if (party->getManager() != user)
    {
        // [DispG1/30] P0：ORIG 0x0820565a `mov %eax,-0xc(%ebp)` 具名局部
        int leaveRet = party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
        if (leaveRet != 1)
        {
            // [R4BigTUDispatchers 2026-09-11 · 语句顺序轴（S2Diff 只读定位）]
            //   ORIG 0x08205676 `call out_from_dungeon` 先于 0x08205681 `call PacketGuardC1`
            //   ⇒ 原码把 PacketGuard 声明写在 out_from_dungeon 之后。我方原顺序相反 ⇒
            //   声明下移（语义不变：notification 在此点之前未被使用）。
            G_GameWorld()->out_from_dungeon(user);
            PacketGuard notification;
            notification.clear();
            notification.put_header(0, 2);
            notification.put_byte(0);
            notification.put_short(1);
            user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
            notification.finalize(true);
            G_GameWorld()->send_all(notification, user);
        }
        return 0;
    }
    if (party->get_quick_party_index() != -1)
    {
        G_CGameManager()->CheckOutQuickParty(party, true);
    }
    party->ReturnToVillage();
    return 0;
}