void IntervalSet::Sort()                               // T @0x085735dc
{
    // ORIG: 空 set 直接跳过；否则 qsort(begin, size, sizeof(Interval), 比较器)
    // ORIG 0x085735e4：`call size; test %eax,%eax; sete %al; test %al,%al; jne`
    // ⇒ 源形为 size()==0 的条件物化（empty() 会 call empty 且无 sete，P194）。
    if (m_intervals.size() == 0)
        return;
    qsort(&m_intervals[0], m_intervals.size(), sizeof(Interval),
          &CompareIntervalForSort);
}