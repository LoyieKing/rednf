void Secu_GoldControl::CheckMoneyRange()
{
    // b28d-31: P183 去指针缓存 —— ORIG 0x08287b80 直接
    // `mov 0x44(%eax),%edx; mov 0x40(%eax),%eax; cmp $0,hi; ja/jb;
    //  cmp $0xfffffffe,lo; ja` 即 64 位无符号 `> 0xfffffffeULL` 展开
    // （逐半指针比较四形不产生该 cmp 链）。语义等价，且消去 -0x10/-0xc 槽。
    if (m_cur.m_gold > 0xfffffffeULL || m_cur.m_sub > 0xfffffffeULL)
    {
        SavetoDB(true, false, true);
    }
