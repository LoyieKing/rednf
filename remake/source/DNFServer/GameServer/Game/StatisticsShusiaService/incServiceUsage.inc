// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
void StatisticsShusiaService::incServiceUsage(int serviceIdx)
{
    // ORIG @0x0860c358。同上：ORIG 0x0860c3d4/0x0860c3da 以
    // `_ZNSt4pairIKiiEC1IRiiEEOT_OT0_`（0x08610c38）就地构造临时后直接
    // insert（无拷贝 ctor，无 -0x1c 中间槽）。
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_serviceUsage.find(serviceIdx);
    if (it == m_serviceUsage.end())
    {
        m_serviceUsage.insert(std::pair<const int, int>(serviceIdx, 1));
    }
    else
    {
        it->second += 1;
    }
}