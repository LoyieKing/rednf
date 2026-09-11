// ---------------------------------------------------------------------------
// CUser::compute_quest_basic_reward_exp（报告全文已读）
// ---------------------------------------------------------------------------
// b28d-65：m_field130 已按 P248档② bool 化（Quest.h）并改裸 if(!field) ⇒ xor$1 对齐。
// [2026-09-11 本轮] 三处 P0 修正（单表达式连乘除 / 删 addRate 具名内联实参 / 尾部
//   `exp += …` 两次内存回写）后：insns 153→150（=ORIG）、gap=0、标量区 -0x24..-0x10
//   与 ORIG 全等、ndiff 20。残余（均 N，语义等价）：①ORIG 0x0866e542 `mov %eax,%ecx`
//   中转 vs 4.4 值直传 `mov %eax,0x8(%esp)`（寄存器角色）；②growthView(-0x2c) 与 sret
//   临时(-0x28) 相对序互反（4.1 先声明浅 / 4.4 先声明深；裸声明提前形实测不翻）；
//   ③x87 临时区深 0xc（-0x3a/-0x3c/-0x3e vs -0x2e/-0x30/-0x32）、帧 0x54 vs 0x44（含
//   N115 的 16B outgoing）。字节直读形（`((unsigned char*)&m_reward)[3]`）实测被物化为
//   `lea+add+movzbl`（150→151）⇒ 需 STExpReward union 字节视角（头改动，非本 TU 写域）。
int CUser::compute_quest_basic_reward_exp(Quest* quest)
{
    if (quest->m_type == 4)                                // [quest+0x8]
    {
        return 0;                                          // [mov $0 早退]
    }

    int levelDelta = get_charac_level() - quest->m_levelMin; // [-0x24]
    int penalty = 100;                                     // [-0x20]
    if (!quest->m_field130)                                // [xor $1 双测 → P248档② 裸形]
    {
        penalty = compute_level_penalty(levelDelta);       // [0x866e439]
    }

    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（getRewardWeight +
    // getRewardExp 两分支，共 3 次）。
    int weight = G_CDataManager()->m_questParameter.getRewardWeight(  // [-0x1c movsbl char]
        static_cast<char>(quest->m_field102));

    int exp = 0;                                           // [-0x18]
    if (quest->m_field101 != 0 || quest->m_field130 != 0)
    {
        exp = G_CDataManager()->m_questParameter.getRewardExp(get_charac_level()); // [0x866e4a8]
    }
    else
    {
        exp = G_CDataManager()->m_questParameter.getRewardExp(quest->m_levelMin);  // [0x866e4c9]
    }

    // [P0 2026-09-11] ORIG 0x0866e4d1..0x0866e508 两段 imul/div 之间**无** exp 槽回写
    //   （首段 `mov %ecx,%eax` 直接进第二段；槽回写只在 0x0866e50e 出现一次）⇒ 原码是
    //   单条连续表达式，非两条赋值语句（两条会多发 `mov %eax,-0x1c` + 回读）。
    exp = exp * weight / 100 * penalty / 100;              // [imul + div 0x64 sar5 ×2]

    int premiumBonus = 0;                                  // [-0x14]
    if (isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f))) // [cmp $0x4f]
    {
        // [P0 2026-09-11] ORIG 0x0866e53d 结果直接 `mov %eax,%ecx`（0x0866e542）当实参，
        //   全函数无 addRate 栈槽 ⇒ 原码为内联右值，无具名局部。
        premiumBonus = setPremiumBonusExp(
            exp, getAddExpRateFromPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)));
    }

    STExpReward growthView;                                // [ctor 085bed52 -0x28]
    int growthBonus = 0;                                   // [-0x10]
    growthView.m_reward = get_growth_power_exp_reward().m_reward;  // [sret 临时 + mov dword]
    // [2026-09-11] ORIG 0x0866e58a 为 `movzbl -0x25(%ebp),%eax; movzbl %al,%eax`
    //   （-0x25 = growthView+3 = m_reward 最高字节**直读**）。
    //   实测负结论：`reinterpret_cast<const unsigned char*>(&growthView.m_reward)[3]`
    //   形被 4.4 物化为 `lea -0x2c(%ebp),%eax; add $0x3,%eax; movzbl (%eax),%eax`
    //   （150→151 insns、ndiff 20→21）⇒ 回滚。要出 ORIG 的 disp 直读需
    //   STExpReward 的 union 字节视角（头改动，非本 TU 写域）⇒ 记 N。
    float growthRate =
        static_cast<float>(static_cast<short>(
            static_cast<unsigned char>(
                static_cast<unsigned int>(growthView.m_reward) >> 24))) /
        100.0f;                                            // [filds %ax / fdiv 100]
    growthBonus =
        static_cast<int>(static_cast<float>(exp) * growthRate); // [fld/fistpl ROUND]

    set_charac_seria_buf_bonus_exp(growthBonus);           // [0x866e5cc]

    // [P0 2026-09-11] ORIG 0x0866e5d1/0x0866e5d7 为两次 `add %eax,-0x18(%ebp)`
    //   （内存回写累加）⇒ 原码是 `exp += …; exp += …; return exp;`（lea 形会变 DIFF）。
    exp += premiumBonus;
    exp += growthBonus;
    return exp;                                            // [-0x18 两加]
}