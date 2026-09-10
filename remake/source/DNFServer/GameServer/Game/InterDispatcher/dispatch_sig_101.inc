int Inter_ReqQueryCharacInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2D] verdict=N：ORIG 各 return 用 %eax（`mov $0x0,%eax`），我方多个 return 0 归并到
    //   %ebx 公共出口（+2 条）；由 gcc 返回常量公共化决定，源码层面无非等价改写，未改。
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    // local_10 = param_3 在 state 守卫后才赋值（ORIG +0x027 mov 0x10(%ebp),-0xc）
    Inter_ReqQueryCharacInfoPktView* pPkt = reinterpret_cast<Inter_ReqQueryCharacInfoPktView*>(data);
    if (pPkt->m_20 != 0)                             // asm +0x033 test je 主块（错误分支落盘）
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x8b,
            static_cast<unsigned char>(
                pPkt->m_20 & 0xff));
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x8b);
    packet.put_byte(1);
    packet.put_int(static_cast<int>(strlen(reinterpret_cast<const char*>(pPkt))));
    // ORIG put_str(char*,int)（Pc 非 const 重载）：实参 cast 成 char* 精确选中该重载。
    packet.put_str(reinterpret_cast<char*>(pPkt),
                   static_cast<int>(strlen(reinterpret_cast<const char*>(pPkt))));
    packet.put_short(                                // msg+0x2a short（cwtl 符号扩展）
        static_cast<int>(pPkt->m_2a));
    packet.put_byte(                                 // msg+0x28（movsbl）
        static_cast<int>(pPkt->m_28));
    packet.put_byte(                                 // msg+0x2c（movsbl）
        static_cast<int>(pPkt->m_2c));
    packet.put_byte(                                 // msg+0x2d（movsbl）
        static_cast<int>(pPkt->m_2d));
    packet.put_byte(                                 // msg+0x2e（movsbl）
        static_cast<int>(pPkt->m_2e));
    packet.finalize(true);
    user->Send(packet);
    return 0;
