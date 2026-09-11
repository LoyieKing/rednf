// GetLinearDataForMonsterAbility  ORIG T 0x08891bae
// diff>=span → high；diff<=0 → low；否则 low + (high-low)*diff/span。
// ===========================================================================
int GetLinearDataForMonsterAbility(int low, int high, int diff, int span)
{
    if (diff >= span)
        return high;
    if (diff <= 0)
        return low;
    return low + (high - low) * diff / span;
}