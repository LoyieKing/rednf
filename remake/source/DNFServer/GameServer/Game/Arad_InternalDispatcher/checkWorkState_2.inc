// ORIG Dispatcher_AradUseAvatarRecharge::checkWorkState 0x0819f9d6 W。
bool Dispatcher_AradUseAvatarRecharge::checkWorkState(CUser* user,
                                                      ::arad::SigAradUseAvatarRecharge& sig)
{
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, (unsigned char)secuErr);
        return false;
    }
    // ORIG 0x0819fa40 !boolcall 尾形（技巧1）。
    if (!Singleton<AvatarRechargeServer>::Get()->CheckConditionUseRecharge(user, &sig))
        return false;
    return true;
}