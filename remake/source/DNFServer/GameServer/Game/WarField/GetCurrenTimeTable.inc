int WarAreaCounter::GetCurrenTimeTable()
{
    // b28d-lane11：ORIG 0x089024d4 `lea -0x18(%ebp),%eax; mov %eax,(%esp); call time`
    // = `time(&t)` 形；循环条件直读 m_num（无 num 局部）、t0/t1 内联无槽（P0）。
    time_t t;
    time(&t);
    struct tm tmbuf;
    struct tm* ptm = localtime_r(&t, &tmbuf);
    int curHour = ptm->tm_hour;
    int i = 0;
    for (; i < m_num - 1; ++i)
    {
        // b28d-lane11：P223 配套——单 continue 产额外 jmp 岛，改正向包裹。
        if (m_timeHourList[i] <= curHour)
        {
            if (m_timeHourList[i + 1] > curHour)
                break;
        }
    }
    return i;
