int CBingo::calBoradMatchLine(int board)
{
    int lines = 0;
    // [DualTailor 2026-09-09] ORIG 0x080cab68 `movl $0x5,-0x24(%ebp)`：写后不读的死局部
    // （报告反编译列作 local_28 undefined4，仅赋 5 无任何读取）⇒ 无语义作用，保留以对齐槽序/帧。
    int lineMax = 5;
    std::bitset<25> bits(static_cast<unsigned long>(board));
    // [DualTailor 2026-09-09] ORIG 0x080cab81/0x080cab88 在循环外声明 rowCount/colCount，
    // 循环体首（0x080caba9/0x080cabb0）再各写一次 0 ⇒ 原码为「循环外定义 + 体内重置」。
    int rowCount = 0;
    int colCount = 0;
    int diag1 = 0;
    int diag2 = 0;
    for (int r = 0; r < 5; ++r)
    {
        rowCount = 0;
        colCount = 0;
        for (int c = 0; c < 5; ++c)
        {
            if (bits[5 * r + c])
            {
                ++rowCount;
            }
            if (bits[5 * c + r])
            {
                ++colCount;
            }
            if (rowCount < c + 1 && colCount < c + 1)
            {
                break;  // ORIG 早退：行列均已不可能凑满
            }
        }
        if (rowCount == 5)
        {
            ++lines;
        }
        if (colCount == 5)
        {
            ++lines;
        }
        if (bits[6 * r])
        {
            ++diag1;
        }
        if (bits[4 * (r + 1)])
        {
            ++diag2;
        }
    }
    if (diag1 == 5)
    {
        ++lines;
    }
    if (diag2 == 5)
    {
        ++lines;
    }
    return lines;
