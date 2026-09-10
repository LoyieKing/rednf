void CUser::send_equip(int slot)
{
    // ORIG 0x0865dd14：state(+0x8cfc4) ∈ {5,8,10,0xc} 且 slot==0x16 → 发给自己；
    // 否则（slot<0xc 或 0x16）→ 广播 make_basic_info(type 0) 给全房间。
    if (slot >= 0xc && slot != 0x16)
        return;
    // 2026-09-10（G3CUser-B，P0 槽-局部律）：ORIG 0x0865dd2c/0x0865dd3e/0x0865dd50/
    //   0x0865dd62 四处**各自** `mov 0x8(%ebp),%eax; mov 0x8cfc4(%eax),%eax;
    //   cmp $..,%eax; je 0x0865de35` —— m_chState 每次经 this 重读（无 state 槽、无
    //   寄存器缓存），4 个条件各自直跳同一落点（inState **无槽**）⇒ 我方
    //   `int state` / `bool inState` 两个具名量 ORIG 均无，删并内联为 4 项 `||`。
    if (m_chState == 0xc || m_chState == 5 || m_chState == 8 || m_chState == 10)
    {
        if (slot != 0x16)
            return;
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
    }
    else
    {
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        G_GameWorld()->send_all(packet, this);
    }
