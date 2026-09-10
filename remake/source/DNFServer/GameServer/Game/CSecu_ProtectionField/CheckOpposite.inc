int CSecu_ProtectionField::CheckOpposite(CUser* user, SECURITY_PROTCTION type,
                                         CUser* opposite)
{
    if (user == 0 || opposite == 0)
        return 1;
    int result = Check(user, type);
    if (result == 0 && opposite->isTradePunishType(0))
        result = 0x73;
    return result;
