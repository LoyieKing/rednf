int Dispatcher_Compound_Avatar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ead40（PacketDispatcher_Impl_1.cpp 0x6c89 区）
    if (user->get_state() <= 2)
    {
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x12);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, (unsigned char)secu);
        return 0;
    }
    // 局部 MSG_COMPOUND_AVATAR（ORIG 栈区 -0x2f..-0xc，0x23 字节；权威布局
    // MSG_COMPOUND_AVATAR.h）。ORIG 各读取直传字段地址 → 绑定
    // get_short(short&)/get_int(unsigned long&)/get_byte(char&) 需同宽指针转换
    //（字段类型按消费侧 _MakeCompoundInfo 的 movzwl/movzbl 读取口径取 unsigned）。
    MSG_COMPOUND_AVATAR msg;
    if (!packet.get_short(msg.m_avatarSlot))
    {
        return LineFunc(0x6c89,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(msg.m_materialSlotA))
    {
        return LineFunc(0x6c8a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&msg.m_materialItemIdA)))
    {
        return LineFunc(0x6c8b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(msg.m_materialSlotB))
    {
        return LineFunc(0x6c8c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&msg.m_materialItemIdB)))
    {
        return LineFunc(0x6c8d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&msg.m_optionIdx)))
    {
        return LineFunc(0x6c8e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(msg.m_slot))
    {
        return LineFunc(0x6c8f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(msg.m_optionCheckFlag))
    {
        return LineFunc(0x6c90,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(msg.m_type))
    {
        return LineFunc(0x6c92,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081eb01e/0x081eb060：`movzwl` 取 u16 后 `cwtl` 扩展 → (int)(short)。
    if (user->CheckItemLock(2, static_cast<int>(static_cast<short>(msg.m_materialSlotA))))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, 0xd5);
        return 0;
    }
    if (user->CheckItemLock(2, static_cast<int>(static_cast<short>(msg.m_materialSlotB))))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, 0xd5);
        return 0;
    }
    ((WongWork::CCompoundAvatar*)GlobalData::s_pCompoundAvatar)
        ->ProcCompound(user, &msg);
    return 0;
}