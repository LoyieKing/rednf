bool BestClearTime::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();
    // ORIG 0x080c8776：data(-0x10)/i(-0xc) 具名；循环条件每轮重读 count
    // （无 count 具名局部）；记录经结构体数组下标访问（ORIG 的 3i*8 强度
    // 归约 + `lea 0x8/0xc(%eax)` 成员 disp）；make_pair 两实参为记录内
    // int&/SIG& 左值。
    SIG_BEST_CLEAR_TIME_VIEW* data =
        reinterpret_cast<SIG_BEST_CLEAR_TIME_VIEW*>(pData);
    int i = 0;
    while (i < data->m_count && i < 1000)
    {
        // ORIG：make_pair<int&,SIG&>(记录.m_key, 记录.m_data) → pair<int,SIG>
        // 拷贝 → map::insert(value_type const&)（三段调用与 ORIG 展开一致）。
        m_map.insert(std::make_pair(data->m_entries[i].m_key,
                                    data->m_entries[i].m_data));
        ++i;
    }
    return true;
