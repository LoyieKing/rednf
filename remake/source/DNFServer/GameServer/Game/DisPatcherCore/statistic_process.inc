// ORIG 0x081bec5e T statistic_process（报告 statistic_process.md ## 反编译 C 全文映射）：
// 卖价估值入 VALUE_STATISTIC_FIELD(6)、货值入 (0xd)，材料对入 CCubeStatistic(0x6a)。
void DisPatcher_BuyItem::statistic_process(CUser* user, BuyItemParam& param)

{
    // ORIG +0x09：输出参 = `add $0xb0,%eax`（param.m_b0 字段，无局部 sellPrice、
    // 无初始化写）；+0x57 `mov 0xb0(%eax),%eax` 读回。两次 AddValueStatistic 各自
    // 现调 GetInstanceValueStatistic（+0x5f/+0x88，无 valueStat 槽，技37）。
    G_Store()->GetSellItemPrice(param.m_item, param.m_a8,
                                (short)param.m_item.m_addInfo2, false, param.m_b0);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)6, user,
                                                   (unsigned int)param.m_b0);
    // ORIG +0x82 `mov 0x80(%eax),%ebx` 单步直落 %ebx；4.4 恒 `mov 0x80,%eax;
    // mov %eax,%ebx` 两段（N18a 反向不可造，register asm("ebx") 绑定反产
    // copy-back 4 段更差）→ 残 2 行 N 类。
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd, user,
                                                   param.m_80);
    GetInstanceCubeStatistic()->collectCubeStatistics(
        param.m_pair.first, param.m_pair.second, user, (CUBE_STATISTIC_FIELD)0x6a);
}
