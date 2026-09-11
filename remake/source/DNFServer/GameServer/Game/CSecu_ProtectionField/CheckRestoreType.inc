// ORIG T 0x08288bf2（报告 CheckRestoreType.md）：恢复惩罚位检查。
int CSecu_ProtectionField::CheckRestoreType(CUser* user, unsigned long protType)
{
    if (user == 0)
        return 1;
    if ((protType & 0x80) != 0 && user->isTradePunishType(0x80))
        return 0xe5;
    return 0;
}