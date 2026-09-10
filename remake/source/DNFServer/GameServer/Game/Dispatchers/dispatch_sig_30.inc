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
    if (party == 0 || party->GetEPLPState() != 0)
    {
        return 0;
    }
    if (party->getManager() == user)
    {
        if (party->get_quick_party_index() != -1)
        {
            G_CGameManager()->CheckOutQuickParty(party, true);
        }
        party->ReturnToVillage();
        return 0;
    }
    if (party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2) != 1)
    {
        PacketGuard notification;
        G_GameWorld()->out_from_dungeon(user);
        notification.clear();
        notification.put_header(0, 2);
        notification.put_byte(0);
        notification.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
        notification.finalize(true);
        G_GameWorld()->send_all(notification, user);
    }
    return 0;
