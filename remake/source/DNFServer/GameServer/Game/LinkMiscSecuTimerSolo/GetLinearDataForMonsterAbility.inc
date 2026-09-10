int GetLinearDataForMonsterAbility(int low, int high, int diff, int span)
{
    if (diff >= span)
        return high;
    if (diff <= 0)
        return low;
    return low + (high - low) * diff / span;
