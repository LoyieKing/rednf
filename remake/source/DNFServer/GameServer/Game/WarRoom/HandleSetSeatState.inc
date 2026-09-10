void WarRoom::HandleSetSeatState(CUser* user, int seat, ENUM_SEAT_STATE state)
{
    PacketGuard packet;
    int ret = 0;
    CUser* outUser = 0;
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    ret = SetSlotState(user, seat, state, &outUser, b1, b2, b3);
    if (ret == 0)
    {
        if (outUser != 0)
        {
            G_GameWorld()->out_from_warroom(outUser);
        }
        if (b1 != 0)
        {
            packet.clear();
            MakeSlotInfo((char*)&packet, seat);
            G_GameWorld()->send_all(packet);
        }
        if (b3 != 0)
            return;
    }
    if (ret > 0)
    {
        packet.put_header(1, 0x5e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
    }
