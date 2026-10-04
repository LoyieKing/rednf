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
        // remake_verify：项目里已有的「锁 ORIG / 允许语义修正」开关。
        //   1 = 验证路线，保持下面的钳 0（与 ORIG 0x08512f9c 一致）。
        //   0 = 现代路线，默认。即用户要的 DNF_REMAKE_MODIFY，默认打开。
        //
        // BUG：一日店货表在 Script.pvf 里。商店 49 的
        // [one a day start time] 是 2012-09-27 06:00，[one a day item]
        // 只有 21 套。天数 = (现在 - 起始) / 86400，起始小时为 6，日界正确。
        // 现在已是第 5120 天，21 套装不下。ORIG 在 day >= size 时不改
        // entry.second，套号永远停在 0。客户端用同一起点算出今天应是
        // day % 21（当前 17），和服务器每分钟下发的 0x155 套号对不上，
        // 领取键全灰，购买包不会发出。6 点只是把 5120 变成 5121，仍然钳死。
#if remake_verify
        if (day >= 0 && (int)script.m_dailyList.size() > day)
            entry.second = day;
#else
        int n = (int)script.m_dailyList.size();
        if (day >= 0 && n > day)
            entry.second = day;
        else if (day >= 0 && n > 0)
            entry.second = day % n;
#endif
    }
    m_oneADayList.push_back(entry);
    script.m_curItemList = script.m_dailyList.at(entry.second);
    return entry.second;
}