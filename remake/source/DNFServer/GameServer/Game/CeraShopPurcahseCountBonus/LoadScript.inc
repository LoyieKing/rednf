// ---------------------------------------------------------------------------
// CeraShopPurcahseCountBonus::LoadScript —— ORIG 0x081abf14
// （docs/class_func_reports/ARAD__CeraShopPurcahseCountBonus/LoadScript.md）：
// return import(...) < 1 即失败返回 true。
// ---------------------------------------------------------------------------
bool CeraShopPurcahseCountBonus::LoadScript()
{
    // ORIG 0x081abf2f setg：import(...) > 0 → false，否则 true。
    if (importCashShopPurcahseBonusItemList(
            "CashShop/PurcahseBonusItem.shp", m_bonusMap) > 0)
        return false;
    return true;
}