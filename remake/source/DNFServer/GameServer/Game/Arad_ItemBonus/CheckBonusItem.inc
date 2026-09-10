bool CeraShopPurcahseCountBonus::CheckBonusItem(int key)
{
    // ORIG 求值序：find 先、end 后（左侧表达式先物化）+ P56 choose 岛形。
    std::map<int, std::vector<PurcahseBonus> >::iterator it = m_bonusMap.find(key);
    if (it == m_bonusMap.end())
        return false;
    return true;
