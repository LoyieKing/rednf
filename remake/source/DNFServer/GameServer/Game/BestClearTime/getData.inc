void BestClearTime::getData(char* pData) const
{
    // ORIG 0x080c883c 结构：data(-0x10)/i(-0xc) 具名 + begin 具名迭代器(-0x18)；
    // 循环体先 i>0x3e7 直接 return（不写 count）；逐字段重算 i*0x18（无 entry
    // 指针缓存，P183）；first 一次 ptEv、second 一次取值后连读 5 个 int。
    SIG_BEST_CLEAR_TIME_VIEW* data =
        reinterpret_cast<SIG_BEST_CLEAR_TIME_VIEW*>(pData);
    int i = 0;
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::const_iterator it =
        m_map.begin();
    while (it != m_map.end())
    {
        if (i > 0x3e7)
            return;
        data->m_entries[i].m_key = it->first;
        const SIG_BEST_CLEAR_TIME_DIFFICULTY& d = it->second;
        data->m_entries[i].m_data.m_time[0] = d.m_time[0];
        data->m_entries[i].m_data.m_time[1] = d.m_time[1];
        data->m_entries[i].m_data.m_time[2] = d.m_time[2];
        data->m_entries[i].m_data.m_time[3] = d.m_time[3];
        data->m_entries[i].m_data.m_time[4] = d.m_time[4];
        ++it;
        ++i;
    }
    data->m_count = i;
