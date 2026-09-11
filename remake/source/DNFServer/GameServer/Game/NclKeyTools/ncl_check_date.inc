// ncl_check_date(char*, char*) —— 当日字符串在 [a, b] 闭区间内返回 1，否则 0。
// ORIG（0x08572e1c）：memset(0x80) → ncl_make_date → strcmp(today,a) < 0 ||
//   strcmp(today,b) > 0 → 0。
int ncl_check_date(char* a, char* b)
{
    int ok = 1;
    char today[0x80];
    std::memset(today, 0, sizeof(today));
    ncl_make_date(today);
    if (std::strcmp(today, a) < 0 || std::strcmp(today, b) > 0)
    {
        ok = 0;
    }
    return ok;
}