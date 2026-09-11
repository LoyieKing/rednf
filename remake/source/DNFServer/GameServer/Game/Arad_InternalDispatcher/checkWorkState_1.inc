// ORIG Dispatcher_AradEmblemCompound::checkWorkState 0x0819fcb4 W：
//   ServiceRestrictManager::isRestricted(user,Enum_1,0x15) → 错误 0xd1；
//   Secu Check(user,0x2a)；最后 EmblemCompoundServer::CheckCondition。
bool Dispatcher_AradEmblemCompound::checkWorkState(CUser* user,
                                                   ::arad::SigAradEmblemCompound& sig)
{
    if (Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0x15))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd1);
        return false;
    }
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, (unsigned char)secuErr);
        return false;
    }
    // ORIG 0x0819fd68 !boolcall 尾形（技巧1）。
    if (!Singleton<EmblemCompoundServer>::Get()->CheckCondition(user, &sig))
        return false;
    return true;
}