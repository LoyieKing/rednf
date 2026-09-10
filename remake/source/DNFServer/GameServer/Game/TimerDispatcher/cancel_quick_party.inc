bool Timer_QuickPartyWaitingPartyMatching::cancel_quick_party(
    QuickParty::CQuickParty* quickParty)
{
    CParty* party = quickParty->get_party_object();
    if (party == 0)
    {
        return false;
    }
    if (G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(
            party, true, static_cast<CUser*>(0)))
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x17f);
        packet.finalize(true);
        party->send_to_party(packet);
        return false;
    }
    return true;
