int CMonsterDrop_Hell::_getGenItemProbIdx(int value)
{
    // [b28d-solo-WongWorkDrop_B] AE：44=44 逐条同形（残差仅重定位标签；
    // p167 CLEAR 3/3）。修法 = for+break 形（jg 跳 break / jge 跳循环外 nop 岛）
    // + unsigned i（4.4 对 unsigned 局部恒内存直比 `cmp -0x10(%ebp),%eax`）。
    // [b28d-38] ORIG 形 = `for (i=0; size()>i; ++i) { e=m_probsSub[i];
    // if (e.v[0]<=value && e.v[1]>=value) break; }`：jg 跳过 break（条件假）、
    // jge 跳循环外 nop 岛（命中）；尾部 size()<=i 再查一次 ⇒ setbe;test;je。
    // i 用 unsigned：4.4 对 unsigned 局部与 size() 比较恒内存直比（ORIG
    // `cmp -0x10(%ebp),%eax`；int 局部会多一条 `mov -0x10,%edx` 中转）。
    unsigned int i = 0;
    for (; m_probsSub.size() > i; ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        if (e.v[0] <= value && e.v[1] >= value)
            break;
    }
    if (m_probsSub.size() <= i)
        return -1;
    return i;
