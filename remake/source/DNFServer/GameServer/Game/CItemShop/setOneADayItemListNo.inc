// ---- setOneADayItemListNo @ 0x8512f9c ----
unsigned int CItemShop::setOneADayItemListNo(ItemShopScript& script)
{
    std::pair<int, int> entry;
    entry.first = script.m_shopId;
    entry.second = 0;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    int diff = curSec - script.m_lastUpdateTime;
    if (diff >= 1) {
        int day = diff / 0x15180;
        if (day >= 0 && (int)script.m_dailyList.size() > day)
            entry.second = day;
    }
    m_oneADayList.push_back(entry);
    script.m_curItemList = script.m_dailyList.at(entry.second);
    return entry.second;
}