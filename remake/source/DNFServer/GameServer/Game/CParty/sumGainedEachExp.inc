unsigned int CParty::sumGainedEachExp(CUser* user, STGainedEachExpData const& data)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE)
    //（缺 1 次调用，语义缺口待属主补）；余差为 N82 帧/槽。@0x85a2488
    int total = 0;
    unsigned int baseExp = static_cast<unsigned int>(data.m_exp);
    float expDivisor = data.m_expPartyDivisor;      // -0x30(%ebp)，fdivs 除数
    int fatigueBonus = 0;                           // -0x2c
    int seriaBonus = 0;                             // -0x28
    int weekendBonus = 0;                           // -0x24

    if (!reinterpret_cast<CBattle_Field*>(&m_battleField)->isWorldMapTowerArea())
    {
        // fildll(exp)*fldl 0x08cbbbd8（33 33 33 33 33 33 d3 3f = double 0.3）→ fistpl
        if (user->getCurCharacFatigueGrownUpBuff() != 0)
        {
            fatigueBonus = static_cast<int>(
                static_cast<double>(static_cast<unsigned long long>(baseExp)) * 0.3);
            total += fatigueBonus;
            user->set_charac_fatigue_buf_bonus_exp(fatigueBonus);
        }

        // 成长之力：ratio = 字节1(v>>8&0xff) / 2.0f（flds 0x08cbbbcc = 2.0f）
        STExpReward growthView;                            // [ctor -0x3c]
        growthView.m_reward =
            user->get_growth_power_exp_reward().m_reward;  // [sret 临时 + mov dword]
        // [判据⑫/ORIG 0x085a2558..0x085a2563] ORIG 双 movzbl（`movzbl -0x3b,%eax;
        //   movzbl %al,%eax; mov %ax,-0x5e; filds -0x5e`）= 源为「short←uchar 两级窄化」
        //   ⇒ 试形：显式 (short)(unsigned char) 链复现两次零扩展 + filds 短装载。
        float seriaRatio = static_cast<float>(
            static_cast<short>(static_cast<unsigned char>(
                (growthView.m_reward >> 8) & 0xff))) / 2.0f;
        // fildll(exp)*fmuls(ratio) → fistpl
        seriaBonus = static_cast<int>(
            static_cast<double>(static_cast<unsigned long long>(baseExp)) *
            seriaRatio);
        user->set_charac_seria_buf_bonus_exp(seriaBonus);
        total += seriaBonus;

        // [2026-09-10 P0/P183] ORIG 该点用 edx 直传 IsEventing（`add $0x34,%edx;
        // mov (%edx),%edx`）且无 -0xc/-0x10 槽 ⇒ 原码为内联表达式，无 repeatEvent 具名局部。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(
                static_cast<CUser*>(0)))
        {
            // ORIG *(int*)(G_CDataManager()+0x520) == ServerParameterScript+0x4b8
            //   = stWeekendBouns.m_bonusExpPercent（importServerParameterScript.md
            //   0x8a8b95b "[weekend bonus exp]" mov %eax,0x4b8(%edx) 实证）。
            // ORIG `mov 0x520(%eax),%edx` 直取（无 weekendPercent 槽）⇒ 实参内联。
            user->setWeekendBonusExp(
                baseExp,
                G_CDataManager()->m_serverParameter.m_weekendBouns4b4.m_bonusExpPercent);
            weekendBonus = user->getWeekendBonusExp();
            total += weekendBonus;
        }

        if (user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))
        {
            // ORIG 0x085a263a `call CUser::getAddExpRateFromPremium(0x4f)`（p167 曾 MISS；
            // 旧码就地取 g_SPremiumInfo[0x4f].m_field44 绕过调用 —— 已补真实调用）。
            total += user->setPremiumBonusExp(
                baseExp,
                user->getAddExpRateFromPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)));
        }
    }

    // 尾段（两路径汇合 +0x1d2）：
    float premiumRate = 0.0f;                       // -0x20
    premiumRate = static_cast<float>(
        user->GetPremiumInfo()->GetAdvantageExpRate());
    premiumRate += static_cast<float>(CServerEvent::GetExpRate());

    // fildll(E); fildll(E); fmuls(premRate); flds 2.0f; fdivrp; faddp → fistpll
    long long newExp64 = static_cast<long long>(
        static_cast<double>(static_cast<unsigned long long>(baseExp)) *
            premiumRate / 2.0f +
        static_cast<double>(static_cast<unsigned long long>(baseExp)));
    unsigned int newExp = static_cast<unsigned int>(newExp64);  // 仅低 32 位回写
    total += static_cast<int>(newExp);

    // fildll(newExp)/fdivs(expDivisor) → fistpl（RC=就近舍入）；party 加成为差额
    int partyBonus = static_cast<int>(newExp) -
                     static_cast<int>(
                         static_cast<double>(static_cast<unsigned long long>(newExp)) /
                         expDivisor);
    user->set_charac_party_bonus_exp(partyBonus);

    total += user->gainExpAsUpperMember(static_cast<int>(newExp));
    total += user->gainGuildSkillExp(static_cast<int>(newExp));
    total += user->gainPowerWarRewardExp(static_cast<int>(newExp));
    return static_cast<unsigned int>(total);
