// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c1d4 | _ZN5CUser13SendCmdPacketENS_11eSendTargetE14ENUM_CMDPACKETi
bool CUser::SendCmdPacket(eSendTarget target, ENUM_CMDPACKET cmd, int param)  // b28c-L5 SIGFIX（P174）
{
    // ORIG 0x867c1d4 块序（技巧 23 goto 钉）：true 路径 put_byte×2 后 **jmp 到位于
    // false(log) 块之后的共享 finalize+SendPacket 段（0x867c279）**；false 块
    // cmd→%ebx 缓存、logFormat、%ebx=0 汇合 0x867c2c4 dtor 尾。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    if (static_cast<unsigned int>(cmd) != 0x41)
        // P0/N96 去绑定；ORIG `mov 0x10,%eax; cmp $0x41,%eax` reg cmp——
        // P209：4.4 对 unsigned 比较恒先载寄存器（signed enum 直折 mem cmp 反形）。
        goto BAD;
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_byte(param);
    goto DO_FIN;
BAD:
    LogManager::logFormat(1, "user.cpp",
                          __PRETTY_FUNCTION__,
                          0x7741, "User %s - CmdID %d invalid", getCurCharacName(),
                          (int)cmd);
    return 0;
DO_FIN:
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return SendPacket(target, packet);
}