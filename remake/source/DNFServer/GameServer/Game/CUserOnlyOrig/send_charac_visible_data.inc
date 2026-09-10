void CUser::send_charac_visible_data()
{
    PacketGuard packet;
    packet.put_header(0, 0x164);
    packet.put_short(get_unique_id() & 0xffff);
    packet.put_byte((int)get_charac_visible_values());
    packet.finalize(true);

    if (G_GameWorld()->IsPVPChannel()) {
        G_GameWorld()->send_all(packet);
        return;
    }
    Village* village = G_GameWorld()->getUserVillage(this);
    if (village == 0) return;
    if (get_area(false) == village->get_gate_area()) {
        Send(packet);
        return;
    }
    switch (get_state()) {                                     // ORIG 决策树：5/8..10/3/default
    case 5:
    case 8:
    case 9:
    case 10: {
        CParty* party = (CParty*)GetParty();
        if (party != 0) party->send_to_party(packet);
    } break;
    case 3: {
        int area = get_area(false);
        G_GameWorld()->send_to_area((int)getCurCharacVill(), area, packet);
    } break;
    default:
        G_GameWorld()->send_all(packet);
        break;
    }
