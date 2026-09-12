// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
// [SemB03 2026-09-12] 链接口径残差：ORIG 0x0860c3xx 迭代器取元素用
//   `_ZNKSt17_Rb_tree_iteratorISt4pairIKiiEEdeEv`（operator*），我方旧形
//   `it->second` 生成 `...ptEv`（operator->）⇒ 同一 pair 的两种取址路径。
//   改 `(*it).second` 复现 operator*（libstdc++ 中 operator-> 恒 =
//   &(operator*()) ⇒ 两者数值恒等，纯符号/形态差异）。
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
        (*it).second += 1;
    }
}