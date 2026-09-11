// ORIG T 0x088a345e：!(level<min || max<level)。
bool BuyUpgradeData::isValidLevel(int level)
{
    bool result = true;
    if (level < getMinLevel() || getMaxLevel() < level)
        result = false;
    return result;
}