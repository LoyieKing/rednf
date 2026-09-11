// [b28d-lane2] verdict=DIFF ndiff=70（ORIG 0x084d3d56）：寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=30（ORIG 0x84d3d56）：P264 .o 重定位/立即数。
int Inter_SecurityCardActivate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    // P324：ORIG `mov 0x10(%ebp),%eax; mov %eax,<S>` 落在 get_state 检查的 return 之后
    //   （我方原把该赋值提到检查之前，多出一次栈槽往返）。
    Inter_SecurityCardActivatePktView* pPkt =
        reinterpret_cast<Inter_SecurityCardActivatePktView*>(data);

    // [H2·2026-09-11 laneC P0] ORIG 084d3d8a `test %al,%al; je 084d3e50` ⇒ 错误包分支在
    //   **函数尾部**（fall-through 是正例分支）；且 084d3e6b `mov $0x0,%eax` 在 ~PacketGuard
    //   之后（域外）。旧写「提前 return 0 + PacketGuard 在 packet 作用域内」使 4.4 用
    //   %ebx 跨清理（mov $0x0,%ebx + mov %ebx,%eax ⇒ n 81 vs ORIG 78）。
    if (pPkt->m_a != '\0')
    {
        user->getSecurityCard()->setActivate(true);
        user->getSecurityCard()->setCertFlag('\x01');

        {
            PacketGuard packet;
            packet.put_header(1, 0xa8);
            packet.put_byte(1);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa8, 1);
    }
    return 0;
}