// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c844 | _ZN5CUser19SendMoneyFullReasonE22ENUM_MONEY_FULL_REASONmm
bool CUser::SendMoneyFullReason(ENUM_MONEY_FULL_REASON reason, unsigned long a, unsigned long b)  // b28d-lane5：ORIG 0x0867c8da 返回 Send 结果
{
    PacketGuard packet;
    packet.put_header(0, 0x33);
    packet.put_byte(reason);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    return Send(packet);
}