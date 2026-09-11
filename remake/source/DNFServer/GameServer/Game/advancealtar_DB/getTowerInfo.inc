// ---- getTowerInfo（ORIG T 0x081328ea，getTowerInfo.md 全文）----
// current(+0x00) 空→0；current->m_buyItems3(+0x6c) find(towerIdx) 未命中→0；
// G_CDataManager()+0x631c（m_altarShop）.getBuyUpgradeDataVec(*current,2) 空→0；
// 向量中 m_nItemId==towerIdx 项未命中→0；getFieldDataPoint(买项.m_short8,
// FT_INDEX=1) 空→0；买项计数 m_short8==0→0；否则 *out1=FieldData+4(m_value)、
// *out2=(int)m_short8，返回 1。
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
}