// ORIG W 0x0830ed48 _ZN8WongWork12CUserPremium25GetAddIndependentDropRateEij
// 二参（unsigned，1..4）选 m_independentDropRates 列；一参（int，0..0x69）选
// g_SPremiumInfo 行；越界返回 0。
// [b28d-solo-CUserPremium] P133/P258 修法：ORIG 0x0830ed48 的两组越界检查是
// **短路或**形（`cmp 0; je bad; cmp 4; jbe ok` 与 `cmp 0; js bad; cmp 0x69;
// jle ok`，两失败块各自独立 mov$0）；旧独立早返形使 4.4 对 level<0 产 jns 反跳。
unsigned int WongWork::CUserPremium::GetAddIndependentDropRate(int level, unsigned int type)
{
    // ORIG 0x830ed48：两组短路或守卫各自独立物化 mov$0 失败块；
    // type-1 落 uchar 槽 -0x9（mov %al,-0x9;movzbl -0x9 回读，P0 具名局部）。
    if (type == 0 || type > 4)
        return 0;
    if (level < 0 || level > 0x69)
        return 0;
    unsigned char idx = static_cast<unsigned char>(type - 1);
    return g_SPremiumInfo[level].m_independentDropRates[idx];
}