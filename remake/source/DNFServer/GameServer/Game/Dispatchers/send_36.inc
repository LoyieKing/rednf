void Dispatcher_GuildCargoPushItem::send(CUser* user, ParamBase& param)     // ORIG 0x081d1316 (36 insn)
{
    stGuildCargoPushItemParam& result = static_cast<stGuildCargoPushItemParam&>(param);
    if (result.m_4 != 0)
    {
        if (result.m_4 == 100 || result.m_4 == 0xb5)
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x109),
                                     static_cast<unsigned char>(result.m_4 & 0xff));
        else
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xfb),
                                     static_cast<unsigned char>(result.m_4 & 0xff));
    }
}