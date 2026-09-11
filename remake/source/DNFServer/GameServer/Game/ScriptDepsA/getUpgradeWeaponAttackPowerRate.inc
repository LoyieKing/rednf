// ORIG T 0x08a39866：索引 = 12*job + 2*type + level（[job][type][level]）；
// type>5 或 level>1 返回 0；否则返回表项 float。
float getUpgradeWeaponAttackPowerRate(ENUM_CHARACTERJOB job, unsigned int type,
                                      unsigned int level)
{
    if (type > 5 || level > 1)
        return 0.0f;
    return upgradeWeaponAttackPowerRate_[static_cast<int>(job)][type][level];
}