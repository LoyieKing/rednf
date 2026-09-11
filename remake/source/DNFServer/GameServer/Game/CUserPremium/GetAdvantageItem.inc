// [b28d-solo-CUserPremium] 修法（P225）：ORIG 0x086adf7c 循环为「尾测 setle
// 物化 + 体首 first==0 出口」形；for&& 合并条件使 4.4 折 jg/bool 物化，
// 改 while+break 后 132=132 ndiff=0。P167 CLEAR（7/7）。
std::vector<std::pair<int, int> > CUserPremium::GetAdvantageItem(int type)
{
    std::vector<std::pair<int, int> > items;
    // ORIG 0x86adf7c：getCurSec+localtime_r 结果不被使用（死调用，保序保留）；
    // entries[type] 每个条件位独立全链（T-C9-1）；表访问以
    // g_SPremiumInfo[type*0x120…] 内联链直读（无 def/entry/helper）；
    // 循环 pair 为默认 ctor + 两字段写 + lvalue push_back（非 make_pair）。
    time_t nowT = (time_t)GlobalData::s_systemTime_.getCurSec();
    tm localTm;
    localtime_r(&nowT, &localTm);
    (void)localTm;
    if (m_entries[type].m_type != 0 && m_entries[type].m_type < 0x6a &&
        g_SPremiumInfo[m_entries[type].m_type].m_kind == 1)
    {
        // [b28d-solo-CUserPremium] P225：ORIG 循环为「尾测 setle 物化 + 体首
        // first==0 出口」形；for&& 合并条件使 4.4 折 jg/bool 物化（ndiff 38）。
        int i = 0;
        while (i < 5)
        {
            if (g_SPremiumInfo[m_entries[type].m_type]
                    .m_advantageItems[i]
                    .first == 0)
                break;
            std::pair<int, int> p;
            p.first = g_SPremiumInfo[m_entries[type].m_type]
                          .m_advantageItems[i]
                          .first;
            p.second = g_SPremiumInfo[m_entries[type].m_type]
                           .m_advantageItems[i]
                           .second;
            items.push_back(p);
            ++i;
        }
    }
    return items;
}