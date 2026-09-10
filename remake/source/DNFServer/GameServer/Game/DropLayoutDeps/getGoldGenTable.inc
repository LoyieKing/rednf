int* CGeneratorCommonData::getGoldGenTable(unsigned char level)
{
    // ORIG T 0x08533424：静态返回区（bss 0x943ded0，函数局部 static 名 ret）；
    // level>0xc8 回退表项 0 并按 0 级取费率；末次经 AutoMarket 折算写回首字。
    static GoldGenEntry ret;
    if (level > 0xc8)
    {
        ret = m_goldGenTable[0];
        level = 0;
    }
    else
    {
        ret = m_goldGenTable[level];
    }
    ret.m_base = G_CDataManager()->GetAutoMarketContitionsControl()->GetGoldDropAmountRate(
        (int)level, ret.m_base);
    return &ret.m_base;
