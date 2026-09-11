void Dispatcher_ItemDictionary::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081dd8b6 T：&param 存局部，两次读 +0x04（0→OkPacket，否则 ErrorPacket）。
    stItemDictionary& p = static_cast<stItemDictionary&>(param);
    if (p.m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b8);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b8, (unsigned char)p.m_4);
}