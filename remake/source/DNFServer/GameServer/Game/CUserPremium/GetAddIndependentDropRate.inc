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
