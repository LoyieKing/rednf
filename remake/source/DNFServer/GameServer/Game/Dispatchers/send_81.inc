// ==== Dispatcher_AvatarOptionChange::send (ORIG 0x081e0e32) ====
void Dispatcher_AvatarOptionChange::send(CUser* user, ParamBase& paramBase)
{
    stChangeAvatarOption& param = *reinterpret_cast<stChangeAvatarOption*>(&paramBase);
    if (param.m_result == 0)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1cc);
        packet.put_byte(1);
        packet.put_short(param.m_dstSlot);
        packet.put_byte((int)(signed char)param.m_optionValue);
        packet.finalize(true);
        user->Send(packet);
        user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                 param.m_srcSlot);
        return;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1cc, (unsigned char)param.m_result);
}