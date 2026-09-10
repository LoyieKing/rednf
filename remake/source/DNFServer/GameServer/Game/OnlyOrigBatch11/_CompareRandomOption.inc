int _CompareRandomOption(const void* a, const void* b)
{
    // [b28d-solo-OnlyOrigBatch11] P0/P183：ORIG 无 sa/sb 具名槽、且第二段比较
    // **完整重算** a/b 两串字节（a 先 b 后）⇒ 两处独立内联表达式；比较用 int
    // （ORIG jge/jle 有符号序，unsigned 会退化为 jae）。探针 /tmp/b28d_s83_probe_cro.cpp
    // q7 与 ORIG 逐条全等（q1/q2 对照：unsigned 出 jae、第二段 b 先出 jge 反向）。
    if ((int)((const RandomOptionSortKeyView*)a)->m_key1 +
            (int)((const RandomOptionSortKeyView*)a)->m_key2 <
        (int)((const RandomOptionSortKeyView*)b)->m_key1 +
            (int)((const RandomOptionSortKeyView*)b)->m_key2)
        return 1;
    if ((int)((const RandomOptionSortKeyView*)a)->m_key1 +
            (int)((const RandomOptionSortKeyView*)a)->m_key2 >
        (int)((const RandomOptionSortKeyView*)b)->m_key1 +
            (int)((const RandomOptionSortKeyView*)b)->m_key2)
        return -1;
    return 0;
