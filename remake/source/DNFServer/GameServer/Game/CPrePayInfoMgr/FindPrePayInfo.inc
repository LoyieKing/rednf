PrePayInfo* CPrePayInfoMgr::FindPrePayInfo(int idx)  // ORIG T 0x082151b8：find 后命中返回 &it->second，否则 NULL
{
    // P28：ORIG 先默认构造迭代器（-0x10 槽）再 find 到临时（-0x1c）拷贝回来
    // （帧 0x38）；一行直构被 4.4 copy-elision 折叠（帧 0x28）。
    std::map<int, PrePayInfo>::iterator it;
    it = m_map.find(idx);
    if (it != m_map.end())
        return &it->second;
    return 0;
}