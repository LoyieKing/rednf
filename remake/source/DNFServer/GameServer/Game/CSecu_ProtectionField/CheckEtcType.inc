int CSecu_ProtectionField::CheckEtcType(CUser* user, SECURITY_PROTCTION type)
{
    if (user == 0)
        return 1;
    if (user->isRestingUser())
        return 0xcf;
    if ((type == (SECURITY_PROTCTION)8 || type == (SECURITY_PROTCTION)9) &&
        !user->isSecurityServiceUsed())
        return 0xe9;
    return 0;
