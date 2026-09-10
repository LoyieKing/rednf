bool CItemUpgrade::isDoEnchant(const Inven_Item& item, char* buf)
{
    unsigned long range = 100;
    int roll = m_rand->randInt(range);
    int bufVal = 0;
    // P191：ORIG 0x085497b3 `jne +0x4b`（bit 置位→尾部 buf[1] 块），fall-through
    // 为 buf[0]。`==0` 负向判定翻转分支极性（免 xor 物化），buf[0] 直落、buf[1]
    // 走 jne 尾岛。残余仅 `movzbl %al`+`test %eax`（4.4 uchar→int 展宽）见 N/P196
    // 注，语义正确。
    // [b27 复证封顶] bit 测试段 ORIG and 后 test %al 8 位直测 vs 4.4 恒
    // movzbl %al,%eax 加宽（P168a③ 同族，uchar 字段位与不可造）。
    if (item.m_random.m_skill0.m_bits.m_flag == 0)
        bufVal = (signed char)buf[0];
    else
        bufVal = (signed char)buf[1];
    if (roll <= bufVal)
        return true;
    return false;
