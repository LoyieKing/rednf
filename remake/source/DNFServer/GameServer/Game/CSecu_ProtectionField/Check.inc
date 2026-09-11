int CSecu_ProtectionField::Check(CUser* user, SECURITY_PROTCTION type)
{
    // ORIG T 0x08288a02（报告 Check.md）
    if (user == 0)
        return 1;

    unsigned int protType = GetProtectionType(type);
    if (protType == 0)
        return 0;

    int ret = 0;
    ret = CheckRestoreType(user, protType);
    if (ret != 0)
        return ret;
    ret = CheckTradingType(user, protType, type);
    if (ret != 0)
        return ret;
    ret = CheckEtcType(user, type);
    if (ret != 0)
        return ret;
    return 0;
}