void CMissionList_Charac::Send_Fail_Mission(CUser& user, short index)
{
    PacketGuard packet;
    packet.put_header(0, 0x157);
    packet.put_short((int)index);
    packet.finalize(true);
    user.Send(packet);
}