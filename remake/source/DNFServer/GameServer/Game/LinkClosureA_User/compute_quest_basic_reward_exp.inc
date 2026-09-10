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

    exp = exp * weight / 100;                              // [imul + div 0x64 sar5]
    exp = exp * penalty / 100;

    int premiumBonus = 0;                                  // [-0x14]
    if (isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f))) // [cmp $0x4f]
    {
        int addRate = getAddExpRateFromPremium(
            static_cast<ENUM_PREMIUM_TYPE>(0x4f));         // [ecx]
        premiumBonus = setPremiumBonusExp(exp, addRate);   // [_ZN15CUserCharacInfo18setPremiumBonusExpEji]
    }

    STExpReward growthView;                                // [ctor 085bed52 -0x28]
    int growthBonus = 0;                                   // [-0x10]
    growthView.m_reward = get_growth_power_exp_reward().m_reward;  // [sret 临时 + mov dword]
    // [G5 2026-09-10 widthscan 线索·实测反劣已回滚] ORIG 0x0866e58a 为
    //   `movzbl -0x25(%ebp),%eax; movzbl %al,%eax`——`-0x25 = -0x28+3` = m_reward 槽
    //   最高字节**直读**（非 `>>24` 移位；widthscan `ext_movzbl orig=6 our=5` 实证）。
    //   纯 cast 字节视图形实测反劣（153→155 insns，gap +3→+5：4.4 未折叠成单条
    //   `movzbl -0x25(%ebp)`）⇒ 需 STExpReward 的 union 双视角（头改动）才可能收益，回滚。
    float growthRate =
        static_cast<float>(static_cast<short>(
            static_cast<unsigned char>(
                static_cast<unsigned int>(growthView.m_reward) >> 24))) /
        100.0f;                                            // [filds %ax / fdiv 100]
    growthBonus =
        static_cast<int>(static_cast<float>(exp) * growthRate); // [fld/fistpl ROUND]

    set_charac_seria_buf_bonus_exp(growthBonus);           // [0x866e5cc]

    return exp + premiumBonus + growthBonus;               // [-0x18 两加]
