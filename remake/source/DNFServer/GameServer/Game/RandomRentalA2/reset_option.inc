unsigned int CRandomOptionItemHandle::reset_option(unsigned long /*itemIdx*/,
                                                   ENUM_RARITY rarity,
                                                   int optionType,
                                                   RandomOption* options)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 210/196；ORIG 0x085f39d2）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （set200 find/end + map1e8 find/end），无具名 script 槽。
    // [DualMisc] ORIG 0x085f39d2：无 slot 具名槽（mov 0x14(%ebp),%eax 每点重取形参）
    // ⇒ P329 删具名局部，内联回实参位。
    if (options->get_option_index((ENUM_RANDOM_OPTION_NUMBER)optionType) == 0)
    {
        return 0x17;
    }

    // set<unsigned long> @+0x200：禁重置道具黑名单；命中即拒绝。
    // [DualMisc] ORIG 0x085f39d2：先求 end() 段（2 参调用）再算 idx、再 find(idx)
    // （3 参调用）；idx 无具名槽（movzbl→-0x54 只是 find 的 const& 临时）
    // ⇒ end() 独立语句先求值（4.4 实参右→左，写成一个表达式恒先算 find）。
    const std::set<unsigned long>::const_iterator setEnd =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_set200.end();
    if (reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
            ->m_set200.find((unsigned long)options->get_option_index(
                (ENUM_RANDOM_OPTION_NUMBER)optionType)) != setEnd)
    {
        return 0x17;
    }

    // map<unsigned char, ResetValues> @+0x1e8：按稀有度取重置区间。
    std::map<unsigned char, ResetValues>::const_iterator rv =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
            ->m_map1e8.find((unsigned char)rarity);
    if (rv == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map1e8.end())
    {
        return 0x13;
    }

    // [Lane13] ORIG 085f3b6d `mov %eax,-0x48; lea -0x48` —— boundIdx 落在**浅槽**
    // （紧贴 randInt 实参区），而具名局部被 4.4 放最深 -0x60（后声明更深）⇒ 内联
    // 为 randInt 实参右值（randInt 取 const&，4.4 自建匿名临时槽）。
    int slotPick =
        m_rand[11].randInt((unsigned long)(rv->second.m_count - 1));

    // [Lane13] P221：ORIG 085f3b6d 段把 CRandomList 构到 **-0x34**（浅），而 4.4 对
    // 具名 chooser 恒放深槽 -0x60；改「构造即传入」全表达式临时链（P178/P278），
    // 4.4 复现 ORIG 浅槽 + get_value 作用在 for_each 返回值临时（sret -0x44）上。
    unsigned int sel =
        std::for_each(rv->second.m_values.begin(), rv->second.m_values.end(),
                      CRandomList<unsigned int>((unsigned int)slotPick))
            .get_value();
    if (sel >= rv->second.m_values.size())
    {
        return 0x13;
    }

    // [SweepRandomRental-2 P183] ORIG 0x085f3bed/0x085f3c0d 两次 ptEv+operator[]
    // 重取 m_values[sel]（无 ResetValue& 缓存）；0x085f3c1d→0x085f3c23 为
    // `mov %eax,-0x10; mov -0x10,%eax; mov %eax,-0x1c` ⇒ 具名 valueBound + 绑定
    // const& 的匿名临时（实参带 cast）。
    unsigned long valueBound =
        (unsigned long)(rv->second.m_values[sel].m_max -
                        rv->second.m_values[sel].m_min - 1);
    // [SweepRandomRental-2 P0] ORIG 0x085f3c41 `mov %eax,%ebx` —— randInt 结果
    // 跨后续 ptEv/operator[] 由 ebx 保活、无栈槽往返 ⇒ 原码无具名 rolled，
    // 内联为加法左操作数；0x085f3c68 `cmpl $0x64; jbe` ⇒ value 为 unsigned。
    unsigned int value =
        (unsigned int)(m_rand[12].randInt((unsigned long)valueBound) +
                       rv->second.m_values[sel].m_min);
    if (MAX_OPTION_VALUE < value)
    {
        value = MAX_OPTION_VALUE;
    }

    options->set_first_value((ENUM_RANDOM_OPTION_NUMBER)optionType,
                             (unsigned char)(value & 0xff));
    options->set_second_value((ENUM_RANDOM_OPTION_NUMBER)optionType,
                              (unsigned char)(value & 0xff));
    return 0;
