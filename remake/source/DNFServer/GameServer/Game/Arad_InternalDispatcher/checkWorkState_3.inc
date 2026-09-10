bool Dispatcher_AvartarHiddenOptionChange::checkWorkState(CUser* user,
                                                          ::arad::SigAvatarHiddenOptionChange& sig)
{
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, (unsigned char)secuErr);
        return false;
    }
    // ORIG 0x0819f956 族同尾形（技巧1）。
    if (!Singleton<AvatarFixedHiddenOptionServer>::Get()->CheckCondition(user, &sig))
        return false;
    return true;
