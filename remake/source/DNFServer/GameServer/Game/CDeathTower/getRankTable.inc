void* CDeathTowerRanking::getRankTable(unsigned int type)
{
    // ORIG 082a774c W：4 张静态表（BSS 0x941f8c0，每表 0x70f4；Init 循环
    // type=1..4 调 getRankTable）懒构造（__cxa_guard + 逐表 C1）；
    // type==0 返回首表，否则 (type-1)*0x70f4 偏移。
    static CDeathTowerRanking s_tables[4];  // 静态初始化由编译器 guard 保护
    if (type == 0)
        return &s_tables[0];
    return &s_tables[type - 1];
