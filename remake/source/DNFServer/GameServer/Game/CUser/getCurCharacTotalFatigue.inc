unsigned short CUser::getCurCharacTotalFatigue() const
{
    // ORIG 0x865776c：total dead-init `movw $0,-0xa`（技巧 8）+ call 物化 `mov %ax,-0xa`
    // (0x865777d)；rate 无槽：test %ax,%ax;setne;test;je 物化（技巧 22，0x8657791-99）。
    unsigned short total = 0;
    total = getCurCharacFatigue();
    // b28d-solo-CUser AE: 条件内联免 bool 槽（ORIG 0x8657791-99 setne;test;je 无槽，
    // 具名 nz 使 4.4 落 -0x9 并推 total 槽 -0xa→-0xc；probe_bool a3/a5）。
    if (m_premium.GetAdvantageFatigueRate() != 0)
    {
        // ORIG 槽反推（声明序即分配序，技巧 21：先声明占近槽）：maxPremium@-0xe、
        // premiumFatigue@-0xc（min 双 lea 0x86577c8-0x86577d2 实参序 (premium,maxPremium)
        // 对应 std::min(a,b) 签名 (T,T) 取 &a,&b → min 第1参=premium@-0xc? lea -0xe 入
        // 0x4(%esp)（第2参）、lea -0xc 入 (%esp)（第1参）⇒ min(premiumFatigue,maxPremium)
        // 保留；added@-0x10、maxTotal@-0x12（后声明远槽——此处三者 ORIG 强制 -0x12<-0xe<-0xc
        // 交叠，maxTotal 声明最先占最远 16bit 槽位序，见下）。
        // 槽序=声明序（4.4 先声明近槽）：premium@-0xc/maxPremium@-0xe/added@-0x10/
        // maxTotal@-0x12（ORIG 0x86577a6-0x86577c4 装载序与赋值分开写）。
        unsigned short premiumFatigue;
        unsigned short maxPremium;
        unsigned short added;
        unsigned short maxTotal;
        maxTotal = getCurCharacTotalMaxFatigue();
        maxPremium = getCurCharacMaxPremiumFatigue();
        premiumFatigue = getCurCharacPremiumFatigue();
        // ORIG 0x86577da-0x86577e1：`_ZSt3minIt` = min<unsigned short>（t=ushort），
        // movzwl (%eax) 解引用 + `add -0xa,%ax`、截断写 added@-0x10。
        added = std::min(premiumFatigue, maxPremium) + total;
        // ORIG 0x86577f2-0x86577fa：total = min(added, maxTotal)（双 lea 形）。
        total = std::min(added, maxTotal);
    }
    return total;  // ORIG 0x86577fe 单点 movzwl -0xa(%ebp)
}