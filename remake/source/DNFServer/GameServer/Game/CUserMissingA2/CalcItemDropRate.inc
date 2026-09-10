int CHARAC_LOAD_MERCENARY::CalcItemDropRate(CUser* user) const
{
    // ORIG 0x081b2d70：`call G_CDataManager; add $0xa8e4,%eax; mov %eax,-0x20`
    // —— m_vec8 先取地址落具名引用局部槽（P0：有槽往返 ⇒ 原码有具名局部），
    // 再作 Find_max_value_lower_N 第二实参（原实现无该局部，实参位置直接
    // 求值出 lea %ecx，帧少 4 且槽号全平移）。
    const std::vector<std::pair<int, std::pair<int, int> > >& vec8 =
        G_CDataManager()->m_mercenarySystem.m_vec8;
    std::pair<int, int> basePair = Find_max_value_lower_N<std::pair<int, int> >(
        m_level, vec8);
    int base = basePair.second;
    float period = GetPeriodBonus();
    float avatar = GetAvatarBonus();
    int term = GetCompetitionPeriodTerm(0);
    // ORIG 0x081b2dd8：`fildl base; fmuls period; fmuls avatar; fildl term;
    // fmulp` ⇒ x87 求值序是 base*period*avatar*term（写 term*base*… 4.4 出
    // fildl term 先 + fmulp 前置，指令序与 ORIG 相反）。
    int rate = (int)((float)base * period * avatar * (float)term);
    if (user != 0 && user->isGMUser())
    {
        static char buffer[0x100];
        sprintf(buffer,
                "\nbase:%d * period:%.2f * avatar:%.2f * term:%d = %d\n",
                base, (double)period, (double)avatar, term, rate);
        SendMessageToCUser(user, buffer);
    }
    // ORIG 0x081b2e7a：`mov $0xffffffff,%eax; jmp` ⇒ 源码是 `return -1;`
    // 直接返回（写回 rate 局部再 return 会多一条 movl 到槽）。
    if (rate < 1)
        return -1;
    return rate;
