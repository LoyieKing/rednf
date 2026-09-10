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
    // [FL5 2026-09-10 P324] ORIG 叶块物理序 = 书写序：case 3 块在 0x0868c0c4、
    // case 5/8/9/10 共用块在 0x0868c105、default 在 0x0868c12d（树：`cmp $5;je 0x16d`
    // /`cmp $3;je 0x12c`/`sub $8;cmp $2;ja 0x195;jmp 0x16d`）。旧书写序（party 先）把
    // party 块排在 ja 的 fall-through，块序与 ORIG 相反。
    switch (get_state()) {                                     // ORIG 决策树：3/5/8..10/default
    case 3: {
        int area = get_area(false);
        G_GameWorld()->send_to_area((int)getCurCharacVill(), area, packet);
    } break;
    case 5:
    case 8:
    case 9:
    case 10: {
        CParty* party = (CParty*)GetParty();
        if (party != 0) party->send_to_party(packet);
    } break;
    default:
        G_GameWorld()->send_all(packet);
        break;
    }
