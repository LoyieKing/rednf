void Dispatcher_BlueMarble::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e61f4
    stBlueMarble& p = static_cast<stBlueMarble&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1f0);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f0,
                                 (unsigned char)(p.m_4 & 0xff));
    }
}