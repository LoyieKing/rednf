// ============================================================================
// ARAD::GetQuarterOfYear —— ORIG T 0x0819e8d0。
// time/localtime 后 quarter=tm_mon/3+1（0x55555556 除魔数）、year=(tm_year+1900)%2000
// （0x10624dd3/sar7 除魔数 = /2000 + imul 0x7d0 取余），sprintf("%02d%d", year, quarter)。
// ============================================================================
// [b29 P0/P329] ORIG 0x0819e8d0：`time(&t)` 取址传参（非 time(0)）；year/quarter
// 无栈槽往返（quarter 留 %ebx: `lea 0x1(%eax),%ebx`，year 中间值留 %esi）⇒ 原码
// 无具名局部、sprintf 实参内联（右→左：quarter 先求，tm_year 只读一次由 CSE 保证）。
void GetQuarterOfYear(char* buf)
{
    time_t t;
    time(&t);
    struct tm tmp = *localtime(&t);
    sprintf(buf, "%02d%d",
            (tmp.tm_year + 1900) - ((tmp.tm_year + 1900) / 2000) * 2000,
            tmp.tm_mon / 3 + 1);
}
