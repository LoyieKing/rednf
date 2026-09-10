int CUser::get_charac_idx(unsigned int characNo) const
{
    // ORIG 0x864df12：prologue 连构 begin→-0x10、end→-0x14（尾循环 cond 双 lea
    // 无重构，技巧 25 具名缓存消重）；声明序 end 后占远槽。
    int idx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itend = m_characList.end();
    for (; it != itend; ++it)
    {
        if ((unsigned int)it->m_characNo == characNo)
            return idx;
        ++idx;
    }
    return -1;
