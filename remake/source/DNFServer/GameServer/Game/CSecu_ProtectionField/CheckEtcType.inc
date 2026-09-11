// ORIG T 0x08288dbc（报告 CheckEtcType.md）：type∈{8,9} 且安全服务未启用 → 0xe9。
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
}