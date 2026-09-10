void Secu_GoldControl::TradeComplete(int subValue, int addValue)
{
    if (subValue > 0)
    {
        SubImportMoney(subValue);
        SubMoney(subValue);
    }
    if (addValue > 0)
    {
        AddMoney(addValue);
    }
