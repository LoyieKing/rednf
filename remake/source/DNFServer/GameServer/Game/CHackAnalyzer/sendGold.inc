// b28d-solo-CHackAnalyzer: AE（正极性 || 守卫 + name!=0 块序 + 真语义 bug
// 修正（addServerHackCnt 实参 (a,money)、GoldTracking 实参 (trackType,a,ip,
// money,fatigue)）+ 内联 fatigue + 无 cast 除法 + 尾部 return 岛形；旧 N82
// 判词作废）——ORIG 0x08278cce。
void Secu_AccountHacking::sendGold(char const* name, unsigned int a,
                                   unsigned int money, unsigned int b,
                                   unsigned char trackType, char* ip)
{
    // ORIG T 0x08278cce（sendGold.md）：同名角色或金额<=500 直返；否则按 name
    // 是否为空分别走 analyze 加计数（附 Hades 金币追踪）/ DB 查询；
    // fatigue<=100 时按比例分档 setPatten(0/1/2)。
    // b28d-solo-CHackAnalyzer: 守卫写正极性 `||`（ORIG 0x08278cee 为
    // mov$1/mov$0 岛形；`!(A)&&B` 反形在 4.4 恒直跳，探针 /tmp/p_or.cpp
    // g1 逐条同形）。
    if (isSameAccountCharacter(name) || money <= 500)
        return;

    // ORIG 0x08278d0f：`cmpl $0,name; je <name==0 块>` ⇒ then 块是 name!=0
    // （P47 块序：先写 makeRequest 分支）。
    if (name != (char const*)0)
    {
        DB_SelectCharacMid::makeRequest(
            (int)((CUser*)m_userP)->GetUID(), 0, name, 0x2ce, money, 0);
    }
    else
    {
        // ORIG 0x08278d6a 实参序 (user, 0x2ce, 1, a, money)（第 4/5 参 = a/money，
        // 旧重建误写 (money, 0)，真语义 bug 已修）。
        ((CUser*)m_userP)->getHackAnalyzer()->addServerHackCnt(
            (CUser*)m_userP, (WongWork::ENUM_HACKTYPE)0x2ce, 1, a, money);
        // ORIG 0x08278db7 实参序 (trackType, a, ip, money, fatigue)（第 2/4 参
        // = a/money，旧重建误写 (money, ..., a)，真语义 bug 已修）；fatigue
        // 内联实参（无具名局部，ORIG 直通 esi）。
        ((XNuclear::CHades*)((CUser*)m_userP)->getHades())
            ->GoldTracking(trackType, a, ip, money,
                           ((CUser*)m_userP)->getCurCharacUsedFatigue());
    }

    // ORIG 0x08278ddc：`&&` 岛形（探针 /tmp/p_and2.cpp h1）；fatigue 为
    // unsigned short ⇒ 16 位比较 `cmp $0x64,%ax`（勿加 (unsigned int) cast）。
    if (m_userP != 0 &&
        ((CUser*)m_userP)->getCurCharacUsedFatigue() <= 100)
    {
        // ORIG 0x08278e0e：两次 fildll 复用同一 8 字节临时槽（-0x28），无
        // unsigned long long 具名局部、无 2^64 修正段 ⇒ 直接 (double)uint 形。
        int ratio = (int)(((double)money / (double)b) *
                          _S_HACK_RATIO_COEF_100F);
        // ORIG 0x08278cce：先无条件 setPatten(GOLD_3)，再按比例分档
        // ratio>0x45→GOLD_0，0x32..0x45→GOLD_1，0x1e..0x31→GOLD_2。
        setPatten(AccountHackingType::GOLD_3, name, a, money);
        if (ratio > 0x45)
            setPatten(AccountHackingType::GOLD_0, name, a, money);
        else if (ratio > 0x31)
            setPatten(AccountHackingType::GOLD_1, name, a, money);
        else if (ratio > 0x1d)
            setPatten(AccountHackingType::GOLD_2, name, a, money);
    }
    // ORIG 尾岛 [nop;jmp]×2 + [nop]（0x23a/0x23b、0x23d/0x23e、0x240）：
    // 显式尾部 return 令 4.4 保留各早退中继块（P63/P285；缺此 return 时
    // 4.4 jump-thread 合并成单 nop，ndiff=2）。
    return;
}