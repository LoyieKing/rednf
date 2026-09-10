bool CharacAdvanceAltarManager::getTowerInfo(int towerIdx, int& outValue,
                                             int& outCount) const
{
    const AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return false;
    }
    const std::map<int, _BuyItem>& buys = cur->m_buyItems3;
    std::map<int, _BuyItem>::const_iterator it = buys.find(towerIdx);
    if (it != buys.end())
    {
        std::vector<BuyUpgradeData>* vec =
            G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(
                cur->m_ridableId, AdvanceAltarShopType::T_2);
        if (vec == 0)
        {
            return false;
        }
        BuyUpgradeData* data = 0;
        for (std::vector<BuyUpgradeData>::iterator vi = vec->begin();
             vi != vec->end(); vi++)
        {
            if (vi->m_nItemId == towerIdx)
            {
                data = &*vi;
                break;
            }
        }
        if (data == 0)
        {
            return false;
        }
        const FieldData* point =
            data->getFieldDataPoint((int)it->second.m_short8, FieldType::FT_INDEX);
        if (point == 0)
        {
            return false;
        }
        if (it->second.m_short8 == 0)
        {
            return false;
        }
        outValue = point->m_value;
        outCount = (int)it->second.m_short8;
        return true;
    }
    return false;
