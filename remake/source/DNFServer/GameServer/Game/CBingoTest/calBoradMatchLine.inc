// calBoradMatchLine @ 0x080cab5a（docs/class_func_reports/CBingo/calBoradMatchLine.md）：
// 统计 25 位棋盘的连线数（5 行 + 5 列 + 2 对角线；对角线1 = bit[r*6]，
// 对角线2 = bit[(r+1)*4]）。
// [DualTailor 2026-09-09 复核] 旧判 N82「槽序不同 ⇒ 封顶」不成立：帧/槽差根因是**源声明序**——
// ORIG 另有写后不读的 int 局部（=5，报告反编译 local_28）且 rowCount/colCount 在循环外声明、
// 循环体首重置；照此改写后帧 $0x64、槽 -0x28..-0xc、尺寸 0x1f2 与 ORIG 全等（DIFF→AE）。
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
}