bool GarenaCheckTimeStamp(unsigned int nowA, unsigned int nowB,
                          unsigned int serverC, unsigned int serverD)
{
    // [b28BM13 P222] ORIG 中层 = `cmp; jne <mov$0 岛>`（正向 == 包裹、else 归
    // return-false 岛，探针 bm13_p6 g2 实证；!=/!(==) 形 4.4 恒产 je 反极性）。
    if (!(nowA + 300 > nowB))       // ja：无符号 >
    {
        if (nowA == serverC)
        {
            if (!(serverD + 300 < nowB))  // jb：无符号 <
                return true;
            return false;
        }
        else
            return false;
    }
    return true;
