void Dispatcher_ItemHyperLinkMessage::send(CUser* user, ParamBase& param)
{
    stItemHyperLinkMessage& p = static_cast<stItemHyperLinkMessage&>(param);
    if (p.m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1a9);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a9,
                                 (unsigned char)(p.m_4 & 0xff));
