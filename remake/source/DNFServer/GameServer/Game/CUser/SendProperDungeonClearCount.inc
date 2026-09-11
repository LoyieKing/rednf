void CUser::SendProperDungeonClearCount()
{
    // ORIG 0x8681833-0x868183b：`cmp $2; setle; test; jne skip` = state<=2 早退
    // （body 在 state>2 执行，此前极性写反已修正）；0x868188a `mov %ax,-0xa;
    // movswl -0xa,%eax` = short 具名槽（pg 基址随之平移 -0x18）。
    if (get_state() <= 2)
        return;
    if (!getCurCharacR())
        return;
    PacketGuard packet;
    packet.put_header(0, 0x10b);
    short cnt = GetProperDungeonClearCount();
    packet.put_short(cnt);
    packet.finalize(true);
    Send(packet);
}