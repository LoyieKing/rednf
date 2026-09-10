int get_seed_from_date()
{
    // ORIG 0x086b1dxx：v 槽先声明(基址 -0x40)、t 次之(-0x14)、localtime_r
    // 返回值落指针槽 pv(-0x10)，字段经 pv-> 间接读（0x086b1d53 `mov
    // -0x10(%ebp),%eax; mov 0x14(%eax),%eax`）。
    // ORIG 0x086b1d7x：return 表达式结果先落临时槽 -0xc(%ebp) 再回读（4.1
    // dead store 具名化）。
    struct tm v;
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm* pv = localtime_r(&t, &v);
    int seed = pv->tm_year * 365 + pv->tm_mon * 30 + pv->tm_mday;
    return seed;
