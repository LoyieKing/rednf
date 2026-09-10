void CUser::gainCurCharacFatigue(int value)
{
    // b28d-G3C 2026-09-10（P211 类型同型化 + P183/P59 形态）：
    // ① ORIG 0x0864f847/0x0864f84e `lea -0x1c(%ebp),%eax / lea -0x18(%ebp),%eax`
    //    ——两个具名槽的**地址直接**喂 std::min<int>(int const&, int const&) ⇒
    //    原码 maxPrefatigue/prefatigue 为 **int**。我方 unsigned 形须为 const int&
    //    造两个临时（多 -0x28/-0x24 槽 + 2 条 `mov %eax,-0x2c/-0x28` 拷贝）。
    // ② ORIG 0x0864f873 `sub %eax,-0x14(%ebp)`（mem 目标）⇒ 原码 `premiumVal -= value;`
    //    （我方原写 `premiumVal - (unsigned int)value` 产 16 位 `sub %ax,%cx`）。
    // ③ ORIG 0x0864f8ba `mov -0x10(%ebp),%eax; mov -0xc(%ebp),%edx;
    //    lea (%edx,%eax,1),%eax; test %eax,%eax; jns` ⇒ normal 侧**无 sum 具名槽**、
    //    if/else 两分支（我方原三目形产 `sar $0x1f; not; and` 位运算夹取 + 独有槽）。
    int maxPrefatigue = 0;
    if (m_premium.GetAdvantageFatigueRate() != 0)
        maxPrefatigue = getCurCharacMaxPremiumFatigue() & 0xffff;

    int prefatigue = getCurCharacPremiumFatigue() & 0xffff;
    // ORIG 0x864f854 调用 std::min<int>(int const&, int const&)（外部函数调用，
    // 非三元/内联）——写显式 std::min<int> 以产出同符号调用点。
    int premiumVal = std::min<int>(prefatigue, maxPrefatigue);
    int remain = (int)premiumVal - value;
    if (remain < 0)
    {
        if ((int)premiumVal > 0)
            setCurCharacPremiumFatigue(0);
        int normal = getCurCharacFatigue() & 0xffff;
        if (normal + remain < 0)
            normal = 0;
        else
            normal = normal + remain;
        setCurCharacFatigue(normal);
    }
    else
    {
        premiumVal -= value;
        setCurCharacPremiumFatigue((unsigned short)premiumVal);
    }
