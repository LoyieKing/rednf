unsigned char CBitManager::GetBitFromLSB(int bits)          // 0x0831c160
{
    // ORIG 0x0831c160：栈上 8 字节掩码表**逐元素赋值**（非初始化列表——P60
    // 列表形变 .rodata 模板+rep movs，ORIG 为 8 条 movb 立即数）；下标 bits-1；
    // -0x10 dead int 零初始化槽（P31）；ShiftRight 第 3 实参 = mov (%eax) =
    // this+0 即 m_data（此前 m_buffer 系误转，0x0831c1b8 实证）。
    unsigned char table[7];
    table[0] = 0x01;
    table[1] = 0x03;
    table[2] = 0x07;
    table[3] = 0x0f;
    table[4] = 0x1f;
    table[5] = 0x3f;
    table[6] = 0x7f;
    unsigned char ret = 0;
    int unused = 0;
    (void)unused;
    unsigned char mask = table[bits - 1];
    ret = m_data[m_size - 1] & mask;
    ShiftRight(bits, m_data);
    return ret;
}