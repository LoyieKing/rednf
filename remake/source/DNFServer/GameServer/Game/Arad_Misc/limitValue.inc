int limitValue(int value, int minValue, int maxValue)
{
    // ORIG：std::max<int>(&value,&minValue) → std::min<int>(&maxValue)
    return std::min(std::max(value, minValue), maxValue);
