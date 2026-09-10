bool CLevelRewardTableMgr::IsOnServer(int serverIdx)
{
    // b28d-solo-CUserDepsB: AE——①P171 照抄：ORIG 0x08687d7b 短路链只有
    // `size()==1 || operator[](0)==0`（无 size()!=0 守卫，空表走 operator[] 越界，
    // 与 Ghidra C 一致）；②P96/P270 非 const iterator 消转换构造；③P28 后置 it++
    // 复现 ppEi 三槽阶梯（it -0x14 / end -0x10 / ++临时 -0xc），74=74 逐条同构。
    if (m_vec8.size() == 1 || m_vec8[0] == 0)
        return true;
    if (m_vec8.empty())
        return false;
    // P96/P270：ORIG 0x08687dd7 begin() 直写 it 槽、无 iterator→const_iterator
    // 转换构造（两处 ppEi/operator!= 均为非 const 版）⇒ 必须用可变 iterator。
    // P28 增量侧：ORIG 0x08687e24 调 `ppEi`（后置版，带独立 sret 临时槽）⇒ 写 it++。
    for (std::vector<int>::iterator it = m_vec8.begin();
         it != m_vec8.end(); it++)
    {
        if (*it == serverIdx)
            return true;
    }
    return false;
