void CPad::reset()
{
    // b28d-37 [N78/N82 封顶，ORIG 0x8?]：ORIG 内联 rep stos 清零 vs 4.4 循环/调用形（P116/N78 族）。
    if (isCertified())
    {
        int checkTime = (int)(m_checkTime28 + 0xb4);
        int curTime = (int)time(0);
        // [b28f-SweepGroupR3 P227] ORIG 0x08599616 在 C1 call **之前**右起求 varargs
        //   并把 `m_certified25` 读入 **callee-saved %ebx** 跨 ctor 保活
        //   （`movzbl 0x25(%eax),%eax; movzbl %al,%ebx` → `mov %ebx,0x10(%esp)`）；
        //   两语句形（具名 trace1/trace2）4.4 只在 ctor 后才求实参 ⇒ 每次重读内存
        //   + 少 push %ebx。改全表达式临时链即复现。
        cMyTrace(__PRETTY_FUNCTION__, 65, 0)(
            "[Before]Goblin Certified check_time(%d), cur_time(%d), Certified(%d)",
            checkTime, curTime, m_certified25);
        if (checkTime < curTime)
            setCertified(false);
        cMyTrace(__PRETTY_FUNCTION__, 74, 0)(
            "[After]Goblin Certified check_time(%d), cur_time(%d), Certified(%d)",
            checkTime, curTime, m_certified25);
    }
