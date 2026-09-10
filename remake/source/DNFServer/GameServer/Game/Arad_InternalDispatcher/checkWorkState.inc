bool Dispatcher_AradAvatarConvert::checkWorkState(CUser* user,
                                                  ::arad::SigAradAvatarConvert& sig)
{
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, (unsigned char)secuErr);
        return false;
    }
    if (!Singleton<AvatarConvertServer>::Get()->CheckCondition(user, &sig))
        return false;
    return true;
