int Inter_Check3rdPartyConcent::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // ORIG +0x26：包基址落 -0xc 具名局部（PacketGuard 因此被推到 -0x18）。
    char* pPkt = data;
    PacketGuard packet;
    packet.put_header(1, 0x1c3);
    packet.put_byte(1);
    packet.put_byte((int)(signed char)*pPkt);   // ORIG movzbl+movsbl 两级
    packet.finalize(true);
    user->Send(packet);
    return 0;
