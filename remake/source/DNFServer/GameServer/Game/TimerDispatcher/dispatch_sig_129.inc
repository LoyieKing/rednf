int Timer_ReturnUserTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == 0)
        return 0;

    if ((unsigned short)user->get_unique_id() != p2)
        return 0;

    user->ResetReturnUser();

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x12d);
    packet.put_int(0);
    packet.finalize(true);
    user->Send(packet);

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (user->get_state() == 5)
    {
        // ORIG 0x0863a631：party!=0 走 fall-through（send_to_party），==0 je 跳尾部 Send 岛。
        if (party != 0)
            party->send_to_party(packet);
        else
            user->Send(packet);
    }
    else
    {
        G_GameWorld()->send_all(packet, user);
    }

    if (party != 0)
    {
        if (!(party->IsReturnUserParty() && party->IsEventCharacParty()))
        {
            G_GameWorld()->send_party_info_to_all(party, 1);
        }
    }
    return 1;
