// [b28d-D9] N95：全部局部槽深恒差 8（4.4 槽基 ebp-8 vs 4.1 顶到 ebp）（ORIG 0x080df676 sz 0x0000012e）
int GreyImage::sampleAlpha(int x, int y)
{
    int xi = x >> 16;
    int yi = y >> 16;
    if (xi < 0 || yi < 0 || xi >= m_width - 1 || yi >= m_height - 1)
    {
        return 0;
    }
    // [P1 round5] 符号轴：ORIG 三处 `sar $0x10`（0x080df76d/0x080df795/0x080df79f）
    //   ⇒ 中间量与最终结果均为有符号；旧稿 unsigned 形产 `shr`（同槽同序仅此一处差）。
    int fx = x & 0xffff;
    int fy = y & 0xffff;
    unsigned char a00 = getAlpha(xi, yi);
    unsigned char a10 = getAlpha(xi + 1, yi);
    unsigned char a01 = getAlpha(xi, yi + 1);
    unsigned char a11 = getAlpha(xi + 1, yi + 1);
    // [P1 round5] P0：ORIG 无 top/bot 具名槽（槽 -0xc f x/-0x8 fy/-0x4..-0x1 a00..a11，
    //   帧 0x2c 无余量）⇒ 内联为单表达式（ORIG 0x080df76b 段 %edx 直接承接上半）。
    return ((((a00 * (0x10000 - fx) + a10 * fx) >> 16) * (0x10000 - fy)) +
            (((a01 * (0x10000 - fx) + a11 * fx) >> 16) * fy) >> 16);
}