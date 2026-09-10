void Dispatcher_ComboSkill::send(CUser* user, ParamBase& param)
{
    stComboSkill& out = static_cast<stComboSkill&>(param);
    if (out.m_result == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fd);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fd,
                                 (unsigned char)(out.m_result & 0xff));
    }
