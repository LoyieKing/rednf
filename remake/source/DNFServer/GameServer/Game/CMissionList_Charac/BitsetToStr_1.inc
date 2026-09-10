void BitsetToStr(const std::bitset<256>& bitset, unsigned char* out)
{
    for (int i = 0; i < 32; ++i)
    {
        // [b28d-solo-CMissionList_Charac 定稿] 同 Ss 重载：累加器 volatile 化后
        // 加法段四段与槽位（i -0x10 / byte -0x11 / j -0xc）逐条对齐 ORIG
        // 0x08a5a91a 段；`int j = 0;` 前置会多一条 `movl $0x0,-0x10(%ebp)`
        // ⇒ 循环变量按 ORIG 写在 for 初始化式内。
        volatile unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            byte = (unsigned char)((bitset[i * 8 + j] << j) + byte);
        out[i] = byte;
    }
