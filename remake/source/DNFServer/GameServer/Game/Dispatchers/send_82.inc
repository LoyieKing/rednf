void Dispatcher_BlueMarbleConfirmInfo::send(CUser* user, ParamBase& param)
{
    stBlueMarbleConfirmInfo& out = static_cast<stBlueMarbleConfirmInfo&>(param);
    if (out.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fc);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fc,
                                 static_cast<unsigned char>(out.m_4 & 0xff));
    }
