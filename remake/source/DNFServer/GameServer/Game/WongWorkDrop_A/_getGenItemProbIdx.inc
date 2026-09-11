// ---- _getGenItemProbIdx — ORIG T 0x085372ee ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：循环内 return 改「命中 break +
// 循环外 size()<=i 复核」（消 4.4 mov$1/mov$0 bool 岛）；元素具名引用 e；
// unsigned i 使 size() 比较恒内存直比（P225/P194 族）。
// [b28d-solo-WongWorkDrop_A] 修法：循环内 return i 改为「命中 break + 循环外
// size()<=i 复核」两段（消 4.4 的 mov$1/mov$0 bool 岛，复现 ORIG
// setbe;test;je 尾块）；元素经具名引用 e 单次 operator[]（ORIG -0xc 槽往返）；
// i 用 unsigned int 使 size() 比较恒内存直比（P225/P194 族）。
int CMonsterDrop::_getGenItemProbIdx(int value)
{
    unsigned int i = 0;
    for (; m_probsSub.size() > i; ++i)                 // +0x24 工作副本表
    {
        stGenItemProb_t& e = m_probsSub[i];
        if (e.v[0] <= value && e.v[1] >= value)
            break;
    }
    if (m_probsSub.size() <= i)
        return -1;
    return i;
}