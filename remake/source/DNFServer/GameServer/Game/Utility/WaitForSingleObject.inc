// ORIG T 0x086b18d4：timeout>10 → 0；循环 timeout 次：*p 为真 → 1；
// 否则 sleep_ext(1,0)；超时返 0。
int WaitForSingleObject(bool* p, int timeout)
{
    if (timeout > 10)
        return 0;
    for (int i = 0; i < timeout; ++i)
    {
        if (*p)
            return 1;
        sleep_ext(1, 0);
    }
    return 0;
}
