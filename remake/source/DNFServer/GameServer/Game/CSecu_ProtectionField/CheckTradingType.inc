// ORIG T 0x08288c46（报告 CheckTradingType.md）：交易惩罚位逐位检查。
int CSecu_ProtectionField::CheckTradingType(CUser* user, unsigned long protType,
                                             SECURITY_PROTCTION type)
{
    if (user == 0)
        return 1;
    if ((protType & 0x01) != 0 && user->isTradePunishType(0x01))
        return 0x72;
    if ((protType & 0x02) != 0 && user->isTradePunishType(0x02))
        return 0x72;
    if ((protType & 0x04) != 0 && user->isTradePunishType(0x04))
        return 0x72;
    if ((protType & 0x08) != 0 && user->isTradePunishType(0x08))
        return 0x72;
    if ((protType & 0x20) != 0 && user->isTradePunishType(0x20))
        return 0xe4;
    if ((protType & 0x40) != 0 && user->isTradePunishType(0x40))
        return 0x72;
    return 0;
}