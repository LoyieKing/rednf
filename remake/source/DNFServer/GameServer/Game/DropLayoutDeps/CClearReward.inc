CClearReward::CClearReward(int count)
    : CItemGenerator(count)
{
    // [b28d-solo-DropLayoutDeps] AE（ORIG 0x0853a968）：194/194 条、帧 0x3c、
    // EH 槽 -0x30/-0x2c 全等（仅 .o 重定位伪差）。修法=m_goldCardCost 清零由
    // memset 改 ORIG 的 for 循环（N78/P214③：4.1 内联逐元素 store）。
    // ORIG 0x0853a968：基类 → vptr 0x08c9a700 → 7×vector(+0x18 步进 0xc，第 7 个在
    // +0x60) → 表 ctor(+0x2a8) → 两个空白卡 vector(+0x23b40/+0x23b78) →
    // 金卡费用表 [201] 清零 → +0x23b38=0.0f → +0x23b3c=0 → +0x23b74=0 →
    // memset(+0x23b4c,0,0x28) → set_disable_drop_random_option_item()。
    // 成员对象由编译器按声明序完成；标量/清零在此补齐。
    for (int i = 0; i <= 200; ++i)          // ORIG 循环清零（非 memset）
        m_goldCardCost[i] = 0;
    m_goldCardCreateRate = 0.0f;
    m_goldCardBlankTotal = 0;
    m_pcRoomBlankTotal = 0;
    memset(m_rewardRatePerMap, 0, sizeof(m_rewardRatePerMap));
    m_table.set_disable_drop_random_option_item();
