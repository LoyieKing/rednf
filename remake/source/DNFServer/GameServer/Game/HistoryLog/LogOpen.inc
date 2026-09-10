FILE* HistoryLog::LogOpen(unsigned int chNo)
{
    int rnd = rand();
    rnd = rnd % 100;
    if (rnd <= 0x5f)
    {
        return 0;
    }

    FILE* file = logfiles_.Acquire();
    if (file == 0)
    {
        return 0;
    }


    // [b28d-round1-L24] P202①/P214③：ORIG 0x084b9b32 的清零是 `movl $0` ×2 +
    // `movw $0`（widthscan：clr_l orig=2 our=0 / clr_w orig=1 our=0 / clr_b orig=0
    // our=10）⇒ 原码是数组初始化式，非 10 条逐字节赋值。
    char date[10] = { '\0' };
    get_str_date(date, 10, '\0');

    char dir[50] = { '\0' };
    sprintf(dir, "%s%s/", "./history/", date);

    struct stat st;
    if (stat(dir, &st) < 0)
    {
        mkdir(dir, 0x1fd);
    }

    char path[127] = { '\0' };
    char* num = NumberToString(chNo, 0);
    sprintf(path, "%s%s.log", dir, num);


    file = fopen(path, "a");
    if (file == 0)
    {
        return 0;
    }
    count_ += 1;
    return file;
