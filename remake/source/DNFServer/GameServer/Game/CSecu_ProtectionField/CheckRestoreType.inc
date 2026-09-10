int CSecu_ProtectionField::CheckRestoreType(CUser* user, unsigned long protType)
{
    if (user == 0)
        return 1;
    if ((protType & 0x80) != 0 && user->isTradePunishType(0x80))
        return 0xe5;
    return 0;
