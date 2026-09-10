void CHackAnalyzer::reportHackInfo()
{
    if (m_hackInfoList.empty())
        return;
    // b28d-solo-CHackAnalyzer: ORIG 0x080f8080 用 const_iterator + 各自独立
    // temp/拷贝构造的 begin/end 局部（P13 缓存 end、P50 直构形），循环为
    // while；循环体先 `info = *it` 死拷贝（P171 照抄），实参改读 `it->`
    // （ORIG 4 次 operator-> 调用，operator* 仅 1 次）。
    std::vector<stHackInfo_t>::const_iterator it = m_hackInfoList.begin();
    std::vector<stHackInfo_t>::const_iterator e = m_hackInfoList.end();
    while (it != e)
    {
        stHackInfo_t info = *it;
        (void)info;
        ReqDBSaveHackInfo(it->m_uid, it->m_characNo, it->m_hackType,
                          it->m_value, 1);
        ++it;
    }
