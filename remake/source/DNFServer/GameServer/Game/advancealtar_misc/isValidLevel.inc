bool BuyUpgradeData::isValidLevel(int level)
{
    bool result = true;
    if (level < getMinLevel() || getMaxLevel() < level)
        result = false;
    return result;
