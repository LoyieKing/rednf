// ORIG T 0x088a2f1a：out=0 起、失败再以 out=1 试一次，返回末次
// getFieldDataPoint 结果。
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
}