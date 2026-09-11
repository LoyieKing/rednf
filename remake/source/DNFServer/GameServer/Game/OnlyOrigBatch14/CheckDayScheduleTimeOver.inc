// ===== _Z24CheckDayScheduleTimeOveril —— ORIG 0x0846c370 =====
// time(&local)（ORIG 0x0846c37a `lea -0xc(%ebp),%eax; call time` 传局部地址，
// 非 time(NULL)）+ day * (-86400) > checkTime（0xfffeae80 == -86400 补码）。
bool CheckDayScheduleTimeOver(int day, long checkTime)
{
    time_t now;
    time(&now);
    return now + static_cast<long>(day) * (-86400L) > checkTime;
}
