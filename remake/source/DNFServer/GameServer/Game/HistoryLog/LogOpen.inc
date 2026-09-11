// [b28d-19] N：N82 —— 帧/槽序（对象临时物化差）；ORIG 0x084b9b32。
FILE* HistoryLog::LogOpen(unsigned int chNo)
{
    // [P1 round5] ORIG 0x084b9bxx：`call rand; mov %eax,%ecx`（**不落槽**，%ecx 直接
    //   承接取模序列）⇒ 原码为单条初始化式 `rand() % 100`；旧稿两句式（`int rnd =
    //   rand(); rnd = rnd % 100;`）多出 -0x14 槽 store/load 对。
    int rnd = rand() % 100;
    // [P1 round5] 控制流轴：ORIG 0x084b9b71 `jle 0x084b9cf6`（跳到**函数末尾**的
    //   `mov $0x0,%eax`）⇒ 原码是「外层 if(rnd > 0x5f){ 主体 } + 末尾单一 return 0」，
    //   非早返 `if (rnd <= 0x5f) return 0;`（后者 4.4 产 `jg + mov $0 + jmp`，
    //   并把 `mov $0x0,%eax` 挪到跳转岛内）。主体整体下移入 if 块，语义不变。
    if (rnd > 0x5f)
    {
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
    // [P1 round5] P0：ORIG 0x084b9c9d `call NumberToString; mov %eax,0xc(%esp)`
    //   直接以内联结果作 sprintf 第 4 实参，**无 num 具名槽**；旧稿 `char* num`
    //   占 -0xc 槽 ⇒ 本函数全部局部槽整体下移 4（date -0x1a→-0x1e / dir -0x4c→
    //   -0x50 / path -0x123→-0x127 / file -0xc→-0x10）。
    sprintf(path, "%s%s.log", dir, NumberToString(chNo, 0));


    file = fopen(path, "a");
    if (file == 0)
    {
        return 0;
    }
    count_ += 1;
    return file;
    }
    return 0;
}