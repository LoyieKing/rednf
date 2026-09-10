float getUpgradeWeaponAttackPowerRate(ENUM_CHARACTERJOB job, unsigned int type,
                                      unsigned int level)
{
    if (type > 5 || level > 1)
        return 0.0f;
    return upgradeWeaponAttackPowerRate_[static_cast<int>(job)][type][level];
