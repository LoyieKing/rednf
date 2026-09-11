// ==== Dispatcher_AvatarOptionChange::send (ORIG 0x081e0e32) ====
void Dispatcher_AvatarOptionChange::send(CUser* user, ParamBase& paramBase)
{
    AocParamView& param = *reinterpret_cast<AocParamView*>(&paramBase);
    if (param.m_errorCode == 0)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1cc);
        packet.put_byte(1);
        packet.put_short(param.m_avatarSlot);
        packet.put_byte((int)(signed char)param.m_option);
        packet.finalize(true);
        user->Send(packet);
        user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                 param.m_consumeSlot);
        return;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1cc, (unsigned char)param.m_errorCode);
}