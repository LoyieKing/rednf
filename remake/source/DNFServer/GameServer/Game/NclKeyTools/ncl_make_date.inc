void ncl_make_date(char* out)
{
    // ORIG：time(&t)（传指针，P43）；tm 字段直读结构槽（非经 p 指针）。
    time_t t;
    std::time(&t);
    struct tm tmBuf;
    struct tm* p = localtime_r(&t, &tmBuf);
    // ORIG 0x85724f9：`je <nop 岛>` + sprintf + 尾 `jmp` + nop（P223 空块占位岛）。
    if (p == 0)
        goto L_done;
    std::sprintf(out, "%04d%02d%02d",
                 tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday);
L_done:;
