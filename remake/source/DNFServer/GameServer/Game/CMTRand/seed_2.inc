void CMTRand::seed()
{
    FILE* fp = fopen("/dev/urandom", "rb");
    unsigned long buf[MT_N];

    if (fp != 0)
    {
        // [b28d-L14 实证] ORIG 0x080da0e6：纯 register 指针/计数/标志循环
        // （p=%edi、n=%esi、ok=%ebx），条件形 `ok && n-- != 0`（setne 先于 sub），
        // 命中条件为 fread(...)!=0（setne %bl）。按 ORIG 逐条转写。
        register unsigned long* p = buf;
        register int n = MT_N;
        register bool ok = true;
        while (ok && n-- != 0)
        {
            ok = fread(p++, sizeof(unsigned long), 1, fp) != 0;
        }
        fclose(fp);
        if (ok)
        {
            seed(buf, MT_N);
            return;
        }
    }

    seed(hash((long)time(0), (long)clock()));
}