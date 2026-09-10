int DisPatcher_DisJointItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->CheckInTrade())
    {
        return 0;
    }
    // ORIG 0x081f9fb8：首 get_short 实参两段 `lea -0x17(%ebp),%eax; add $0xd,%eax`
    // （技巧26 blob 覆写形：13B 前导的对象基址 -0x17，slot 成员 +0xd = -0xa）。
    // ORIG msg 基址 -0x17（奇数=align1，15B char 数组），实参两段
    // `lea -0x17(%ebp); add $0xd`（技巧26 blob 指针破 lea 折叠）；尾读
    // `movzwl -0xa(%ebp)` 单级（常量地址折叠）。
    struct JointMsg { char m_pad[0xd]; short m_slot; } __attribute__((packed)) msg;
    if (!packet.get_short(*reinterpret_cast<short*>(msg.m_pad + 0xd)))
    {
        return LineFunc(0x88d0, __PRETTY_FUNCTION__, 0, 0);
    }
    char type = 0;
    if (!packet.get_byte(type))
    {
        return LineFunc(0x88d2, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned short target = 0xffff;
    if (!packet.get_short(target))
    {
        return LineFunc(0x88d6, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 尾 `call; leave; ret`（结果直落 %eax 无 `mov $0x0,%eax`）= 尾 return。
    return DisPatcher_DisJointItem::_disjoint(user,
                                              (int)msg.m_slot,
                                              (int)(char)type,
                                              (ENUM_CMDPACKET)0x1c, (CUser*)0,
                                              (unsigned short)target);
