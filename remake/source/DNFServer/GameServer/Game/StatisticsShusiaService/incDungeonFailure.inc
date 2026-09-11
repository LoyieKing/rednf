// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF(N82 封顶) → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
void StatisticsShusiaService::incDungeonFailure(int dungeonIdx)
{
    // ORIG @0x0860c1ba。
    // [P0/N82 消指令] ORIG 0x0860c2b4 直接 call
    // `_ZNSt4pairIKiiEC1IRiiEEOT_OT0_`（= 0x08610c38 同族，pair<int const,int>
    // 的转发 ctor）把临时对象作 insert 的实参；旧形 `std::make_pair(idx,1)` 造出
    // pair<int,int> 再经转换 ctor 拷进 value_type 临时 ⇒ 多一次构造 call + 8B 槽
    // （帧 0x40 vs ORIG 0x30）。改成 value_type 临时直传。
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_dungeonFailure.find(dungeonIdx);
    if (it == m_dungeonFailure.end())
    {
        m_dungeonFailure.insert(std::pair<const int, int>(dungeonIdx, 1));
    }
    else
    {
        it->second += 1;
    }
}