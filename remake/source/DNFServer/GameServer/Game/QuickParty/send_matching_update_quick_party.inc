void CQuickParty::send_matching_update_quick_party(char poolIdx,
                                                   unsigned short masterUid,
                                                   bool changed)
{
    for (int i = 0; i <= 3; ++i)
    {
        CParty* party = get_party_object(i);
        if (party == 0)
            continue;
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x180);
        packet.put_byte(poolIdx);
        packet.put_short(masterUid);
        packet.put_byte(changed ? 1 : 0);
        packet.finalize(true);
        party->send_to_party(packet);
    }
