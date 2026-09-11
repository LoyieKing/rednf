// [b28d-62] 修：P167 info 指针缓存（1×operator-> 替 4×ptEv）。ORIG 0x0860cc38
int CUserPlayTimeStatisticMgr::UpdateUserPlayTime(unsigned int accId, int playSec)
{
    // ORIG 0x0860cc38：命中 → +0 累加 playSec、+4 计数 +1；未命中 → 插入
    // {playSec, 1}。
    std::map<unsigned int, stUserPlayInfo_t>::iterator it = m_userPlayInfo.find(accId);
    if (it != m_userPlayInfo.end())
    {
        // [b28d-62] P167/P36：ORIG 0x0860cc90 只发 1 次 operator-> 并把
        //   `&it->second` 缓存到 -0xc（info 指针局部），逐字段经 info-> 访问；
        //   直写 4 次 it->second 会多发 3 次 ptEv call。
        stUserPlayInfo_t* info = &it->second;
        info->m_playSec = info->m_playSec + playSec;
        info->m_count = info->m_count + 1;
    }
    else
    {
        stUserPlayInfo_t info;
        // [b28d-63] 修：ORIG 0x0860ccb6 先 `movl $0x1,-0x38`（m_count=1）再
        //   0x0860ccc0 写 m_playSec；源码赋值序照 ORIG。
        info.m_count = 1;
        info.m_playSec = playSec;
        m_userPlayInfo.insert(std::make_pair(accId, info));
    }
    return 1;
}