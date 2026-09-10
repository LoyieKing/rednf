FieldData* BuyUpgradeData::getMinLevelFieldDataPoint(FieldType::T ft, int& outLevel)
{
    outLevel = 0;
    FieldData* point = getFieldDataPoint(outLevel, ft);
    if (point == 0)
    {
        outLevel = 1;
        point = getFieldDataPoint(outLevel, ft);
    }
    return point;
