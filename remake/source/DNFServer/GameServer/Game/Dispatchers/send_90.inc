void Dispatcher_Teleport::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d087c 两处形：①`test %eax,%eax; jne <guard 段>` —— err==0 正常路径是
    // fall-through、SendCmdErrorPacket 上报体在函数物理尾（技巧17/23，同 SkillInit/
    // RefundSkill::send）；②读 m_10 时是 `movzbl 0x10(%eax),%eax; movsbl %al,%eax`
    // 两段（技巧15 signedness 两步：字段零扩展后再符号扩展成 int），我方原写 unsigned
    // 提升只到 movzbl ⇒ 少一段且语义应为有符号。
    stTeleportParam& out = static_cast<stTeleportParam&>(param);
    if (out.m_4 == static_cast<int>(0x7fffffff))
    {
        return;
    }
    if (out.m_4 == 0)
    {
        PacketGuard itemPacket;
        itemPacket.put_header(0, 0xe);
        itemPacket.put_byte(static_cast<int>(static_cast<signed char>(out.m_10)));
        itemPacket.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket(
            static_cast<INVEN_TYPE>(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<int>(static_cast<signed char>(out.m_10)))),
            out.m_8, itemPacket);
        itemPacket.finalize(true);
        user->Send(itemPacket);

        PacketGuard packet;
        packet.put_header(1, 0xf1);
        packet.put_byte(1);
        packet.put_short(out.m_8);
        packet.put_int(out.m_14);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf1),
                                 static_cast<unsigned char>(out.m_4));
    }
